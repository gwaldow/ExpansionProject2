

#include "TWAPawn.h"

#include "EngineUtils.h"

#include "TWAUtilities.h"
#include "TWAController.h"
#include "CameraLimitVolume.h"
#include "TWACheckpoint.h"
#include "Resetable.h"
#include "TWAGameModeBase.h"
#include "Flammable.h"

void ATWAPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATWAPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (ATWAController* controller = Cast<ATWAController>(GetController()))
	{
		controller->SetupPlayerInputComponent(PlayerInputComponent);
	}

	RecomputeViewTargets();

	SetActorRotation(FRotator::ZeroRotator);
}

void ATWAPawn::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (bGameStarted)
	{
		for (int i = ViewTargets.Num() - 1; i >= 0; --i)
		{
			if (ViewTargets[i] == nullptr || !IsValid(ViewTargets[i]))
			{
				ViewTargets.RemoveAtSwap(i);
			}
		}

		if (ViewTargets.IsEmpty())
		{
			if(PlayerDeadTimestamp <= 0.0f)
			{
				OnPlayerDead();
				PlayerDeadTimestamp = Utils::GetGameTime();
			}
			else 
			{
				if (!Utils::IsRecentEvent(PlayerDeadTimestamp, DeadFadeOutDelay) && !bTriggeredFadeOut)
				{
					bTriggeredFadeOut = true;
					OnDeathFadeOut();
				}

				if (!Utils::IsRecentEvent(PlayerDeadTimestamp, RespawnDelay))
				{
					if (LevelCameraLimit != nullptr && LevelCameraLimit->Checkpoint != nullptr)
					{ 
						LevelCameraLimit->bExited = false;
						LevelCameraLimit->Checkpoint->Respawn();
						PlayerDeadTimestamp = -1.0f;
						bTriggeredFadeOut = false;
						SetActorLocation(GetTargetViewLocation(), false);
						OnPlayerRespawn();

						if (ATWAGameModeBase* gameMode = Utils::GetGameMode())
						{
							for (UObject* object : gameMode->ResetableList)
							{
								if (IsValid(object))
								{
									IResetable::Execute_Reset(object);
								}
							}

							if (ATWAController* controller = Cast<ATWAController>(GetController()))
							{
								controller->SnapClutterToGround();
							}
						}
					}
				}
			}
		}

		FVector viewLocation = Utils::Approach(GetActorLocation(), GetTargetViewLocation(), CameraTargetLocationApproach, deltaTime);

		SetActorLocation(viewLocation, false);
	}
}

void ATWAPawn::StartGame()
{ 
	bGameStarted = true;

	for (TActorIterator<ACameraLimitVolume> cameraLimitVolume(GetWorld()); cameraLimitVolume; ++cameraLimitVolume)
	{
		if (ACameraLimitVolume* cameraLimit = (*cameraLimitVolume))
		{
			if (cameraLimit->EncompassesPoint(GetActorLocation(), 100.0f))
			{
				LevelCameraLimit = cameraLimit;
				break;
			}
		}
	}

	if(LevelCameraLimit != nullptr && LevelCameraLimit->Checkpoint)
	{
		LevelCameraLimit->Checkpoint->Respawn();
		RecomputeViewTargets();
	}
	SetActorLocation(GetTargetViewLocation(), false);

	if (ATWAController* controller = Cast<ATWAController>(GetController()))
	{
		controller->SnapClutterToGround();
	}
}

FVector ATWAPawn::GetTargetViewLocation() const
{
	int targetCount = 0;
	FVector targetViewLocation = FVector::Zero();
	for (AActor* viewTarget : ViewTargets)
	{
		if (viewTarget)
		{
			++targetCount;

			FVector offset = FVector::ZeroVector;

			if (UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(viewTarget->GetRootComponent()))
			{
				if (primComp != nullptr && primComp->IsSimulatingPhysics())
				{
					TWEAKABLE float CameraVelocityOffsetTime = 0.65f;
					offset = primComp->GetComponentVelocity() * CameraVelocityOffsetTime;
				}
			}

			targetViewLocation += viewTarget->GetActorLocation() + offset;
		}
	}

	FVector viewLocation = GetActorLocation();
	float z = viewLocation.Z;

	if (targetCount > 0)
	{
		targetViewLocation /= (float)targetCount;

		for (ACameraLimitVolume* cameraLimit : CameraLimits)
		{
			if (!cameraLimit->IsInsideLimits(targetViewLocation))
			{
				cameraLimit->ClampViewTargetLocation(targetViewLocation);
			}
		}

		viewLocation = targetViewLocation;
	}

	viewLocation.Z = z;

	return viewLocation;
}

void ATWAPawn::RecomputeViewTargets()
{
	bool bHadNoTarget = ViewTargets.IsEmpty();
	ViewTargets.Empty();
	CameraLimits.Empty();

	TArray<ACameraLimitVolume*> cameraLimitVolumes;

	for (TActorIterator<ACameraLimitVolume> cameraLimitVolume(GetWorld()); cameraLimitVolume; ++cameraLimitVolume)
	{
		if ((*cameraLimitVolume) != nullptr)
		{
			cameraLimitVolumes.Add(Cast<ACameraLimitVolume>(*cameraLimitVolume));
		}
	}

	bool bIsFlaming = false;

	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (FActorIterator It(World); It; ++It)
		{
			AActor* actor = *It;
			if (actor->ActorHasTag("CameraViewTarget"))
			{
				ViewTargets.Add(actor);

				for (ACameraLimitVolume* cameraLimit : cameraLimitVolumes)
				{
					if(cameraLimit && !cameraLimit->bExited)
					{
						TArray<AActor*> overlappingActors;
						cameraLimit->GetOverlappingActors(overlappingActors);

						if (overlappingActors.Contains(actor))
						{
							CameraLimits.AddUnique(cameraLimit);
						}

						if(actor->Implements<UFlammable>())
						{
							bIsFlaming = bIsFlaming || IFlammable::Execute_IsFlaming(actor);
						}
					}
				}
			}
		}
	}

	if (bHadNoTarget && !bEverHadTargets && ViewTargets.Num() > 0)
	{
		// SNap to position
		SetActorLocation(GetTargetViewLocation(), false);

		bEverHadTargets = true;
	}

	if (CameraLimits.Num() > 0 && !bIsFlaming)
	{
		if(LevelCameraLimit != CameraLimits[CameraLimits.Num() - 1])
		{
			LevelCameraLimit = CameraLimits[CameraLimits.Num() - 1];
			OnCheckpointReached();
		}
	}
}

void ATWAPawn::OnFireStateChanged()
{
	RecomputeViewTargets();
}