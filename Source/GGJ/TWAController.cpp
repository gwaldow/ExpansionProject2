
#include "TWAController.h"

#include "Curves/CurveFloat.h"
#include "Curves/RichCurve.h"
#include "DrawDebugHelpers.h"

#include "TWAUtilities.h"
#include "TWAGameModeBase.h"
#include "Windable.h"
#include "TWAPawn.h"

void ATWAController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("WindForward", this, &ATWAController::WindForward);
	PlayerInputComponent->BindAxis("WindRight", this, &ATWAController::WindRight);

	PlayerInputComponent->BindAction("BoostWind", IE_Pressed, this, &ATWAController::BoostWindPressed);
	PlayerInputComponent->BindAction("BoostWind", IE_Released, this, &ATWAController::BoostWindReleased);
}

void ATWAController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if(GetWorld() && !GetWorld()->IsPaused())
	{
		TWEAKABLE FVector2D ForcedWindInput = FVector2D(0.0f, 0.0f);

		if (!ForcedWindInput.IsNearlyZero())
		{
			WindInput2D = ForcedWindInput;
		}


		ATWAGameModeBase* gameMode = Utils::GetGameMode();

		// Regen boost
		if (WindBoostBurnedOutDuration > 0.0f)
		{
			WindBoostBurnedOutDuration = FMath::Max(WindBoostBurnedOutDuration - deltaTime, 0.0f);
		}

		float windBoostElapsedTime = Utils::ElapsedTime(WindBoostStartedTime);

		if (bWindWantsBoost && BoostCurveMultiplier != nullptr && windBoostElapsedTime > BoostCurveMultiplier->FloatCurve.GetLastKey().Time)
		{
			bWindWantsBoost = false;
			WindBoostBurnedOutDuration = BoostRegenDuration;
			OnBoostEnded.Broadcast(windBoostElapsedTime);
		}

		float windBoostRatio = BoostCurveMultiplier && bWindWantsBoost ? BoostCurveMultiplier->GetFloatValue(windBoostElapsedTime) : 1.0f;

		float windInputRatio = bWindWantsBoost ? 1.0f : 0.0f;
		if(!WindInput2D.IsNearlyZero())
		{
			float desiredWindAngle = FMath::RadiansToDegrees(FMath::Atan2(WindInput2D.Y, WindInput2D.X));
			if (FMath::IsNearlyZero(WindSpeed, 0.01f))
			{
				WindAngle = desiredWindAngle;
			}
			else
			{
				WindAngle = Utils::ApproachAngle(WindAngle, desiredWindAngle, WindAngleApproach, deltaTime);
			}

			windInputRatio = WindInput2D.Size();
		}

		float targetWindSpeed = windInputRatio * windBoostRatio;
		WindSpeed = Utils::Approach(WindSpeed, targetWindSpeed, WindSpeedApproach, deltaTime);
		
		float windAngleRad = FMath::DegreesToRadians(WindAngle);
		FVector windForce = FVector(FMath::Cos(windAngleRad), FMath::Sin(windAngleRad), 0.0f) * WindSpeed;

		if(!windForce.IsNearlyZero(0.01f))
		{
			if (gameMode)
			{
				for (UObject* object : gameMode->WindableList)
				{
					if(object != nullptr)
					{
						IWindable::Execute_Wind(object, windForce);
					}
				}
			}
		}

		if (ATWAPawn* pawn = Cast<ATWAPawn>(GetPawn()))
		{
			TWEAKABLE float WindForceDebugDrawMultiplier = 100.0f;
			//DrawDebugDirectionalArrow(GetWorld(), pawn->GetActorLocation() + VecZ(100.0f), pawn->GetActorLocation() + VecZ(100.0f) + windForce * WindForceDebugDrawMultiplier, 25.0f, FColor::Blue);
		}

		if (gameMode)
		{
			FBox clutterLimitBox = GetWindClutterLimits();
			FVector clutterLimitBoxCenter = clutterLimitBox.GetCenter();
			FVector clutterLimitBoxExtent = clutterLimitBox.GetExtent();

			for (UObject* object : gameMode->WindClutter)
			{
				if (AActor* actor = Cast<AActor>(object))
				{
					if (UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(actor->GetRootComponent()))
					{
						FVector velocity = primComp->GetComponentVelocity();
						
						TWEAKABLE float MaxVelocity = 2550.0f;
						if (velocity.SizeSquared() > MaxVelocity * MaxVelocity)
						{
							primComp->SetPhysicsLinearVelocity(velocity.GetSafeNormal() * MaxVelocity);
						}
					}

					bool bClampToGround = false;
					FVector newLocation = actor->GetActorLocation();
					if(!clutterLimitBox.IsInsideOrOn(newLocation))
					{	
						FVector toClutter = newLocation - clutterLimitBoxCenter;

						if (toClutter.X < -clutterLimitBoxExtent.X)
						{
							newLocation.X = clutterLimitBoxCenter.X + clutterLimitBoxExtent.X;
							newLocation.Z = clutterLimitBoxCenter.Z + clutterLimitBoxExtent.Z;
							bClampToGround = true;
						}
						else if (toClutter.X > clutterLimitBoxExtent.X)
						{
							newLocation.X = clutterLimitBoxCenter.X - clutterLimitBoxExtent.X;
							newLocation.Z = clutterLimitBoxCenter.Z + clutterLimitBoxExtent.Z;
							bClampToGround = true;
						}

						if (toClutter.Y < -clutterLimitBoxExtent.Y)
						{
							newLocation.Y = clutterLimitBoxCenter.Y + clutterLimitBoxExtent.Y;
							newLocation.Z = clutterLimitBoxCenter.Z + clutterLimitBoxExtent.Z;
							bClampToGround = true;
						}
						else if (toClutter.Y > clutterLimitBoxExtent.Y)
						{
							newLocation.Y = clutterLimitBoxCenter.Y - clutterLimitBoxExtent.Y;
							newLocation.Z = clutterLimitBoxCenter.Z + clutterLimitBoxExtent.Z;
							bClampToGround = true;
						}

						newLocation.Z = FMath::Clamp(newLocation.Z, clutterLimitBoxCenter.Z - clutterLimitBoxExtent.Z, clutterLimitBoxCenter.Z + clutterLimitBoxExtent.Z);
					}

					actor->SetActorLocation(newLocation);
					if (bClampToGround)
					{
						actor->SetActorLocation(Vec2D(newLocation) + VecZ(clutterLimitBoxCenter.Z - clutterLimitBoxExtent.Z), true);
					}
				}
			}
		}
	}
}

void ATWAController::WindForward(float value)
{
	WindInput2D.X = value;
}

void ATWAController::WindRight(float value)
{
	WindInput2D.Y = value;
}

void ATWAController::BoostWindPressed()
{
	if(!bWindWantsBoost && WindBoostBurnedOutDuration <= 0.0f)
	{
		bWindWantsBoost = true;
		WindBoostStartedTime = Utils::GetGameTime();
		OnBoostStarted.Broadcast();
	}
}

void ATWAController::BoostWindReleased()
{
	if (bWindWantsBoost && BoostCurveMultiplier != nullptr)
	{
		bWindWantsBoost = false;
		WindBoostBurnedOutDuration = FMath::Max(MapClamped(Utils::ElapsedTime(WindBoostStartedTime), 0.0f, BoostCurveMultiplier->FloatCurve.GetLastKey().Time, 0.0f, BoostRegenDuration), MinimumReboostInterval);
		OnBoostEnded.Broadcast(MapClamped(Utils::ElapsedTime(WindBoostStartedTime), 0.0f, BoostCurveMultiplier->FloatCurve.GetLastKey().Time, 0.0f, BoostRegenDuration));
	}
}

FBox ATWAController::GetWindClutterLimits() const
{
	TWEAKABLE float EyeXOffset = 1900.0f;
	TWEAKABLE float EyeZOffset = -100;
	TWEAKABLE float EyeXLimit = 2500.0f;
	TWEAKABLE float EyeYLimit = 4000.0f;
	TWEAKABLE float EyeZMinLimit = -200.0f;
	TWEAKABLE float EyeZMaxLimit = 350.0f;


	FVector eyeLocation;
	FRotator eyeRotation;
	Utils::GetPawn()->GetActorEyesViewPoint(eyeLocation, eyeRotation);

	// WTF?
	eyeLocation += /*eyeRotation.RotateVector*/(FVector(EyeXOffset, 0.0f, EyeZOffset));

	FBox box = FBox(eyeLocation - FVector(EyeXLimit, EyeYLimit, -EyeZMinLimit), eyeLocation + FVector(EyeXLimit, EyeYLimit, EyeZMaxLimit));

	//DrawDebugBox(GetWorld(), box.GetCenter(), box.GetExtent(), FColor::Blue, false, 0.1f);

	return box;
}

void ATWAController::SnapClutterToGround()
{
	FBox clutterLimitBox = GetWindClutterLimits();
	FVector clutterLimitBoxCenter = clutterLimitBox.GetCenter();
	FVector clutterLimitBoxExtent = clutterLimitBox.GetExtent();

	if(ATWAGameModeBase* gameMode = Utils::GetGameMode())
	{
		for (UObject* object : gameMode->WindClutter)
		{
			if (AActor* actor = Cast<AActor>(object))
			{
				FVector actorLocation = actor->GetActorLocation();
				actor->SetActorLocation(Vec2D(actorLocation) + VecZ(clutterLimitBoxCenter.Z - clutterLimitBoxExtent.Z), true);

				if (UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(actor->GetRootComponent()))
				{
					primComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
					primComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
				}
			}
		}
	}
}