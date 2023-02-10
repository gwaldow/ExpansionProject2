

#include "CameraLimitVolume.h"

#include "EngineUtils.h"
#include "Components/BrushComponent.h"

#include "TWAUtilities.h"
#include "TWAPawn.h"
#include "TWACheckpoint.h"

void ACameraLimitVolume::BeginPlay()
{
	Super::BeginPlay();

	if (UBrushComponent* brushComp = GetBrushComponent())
	{
		brushComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore); // Ignore TWADynamic
		brushComp->OnComponentBeginOverlap.AddDynamic(this, &ACameraLimitVolume::OnCameraLimitBeginOverlap);
		brushComp->OnComponentEndOverlap.AddDynamic(this, &ACameraLimitVolume::OnCameraLimitEndOverlap);

		TriggerOverlapEvents();
	}

	if (Checkpoint == nullptr)
	{
		for (TActorIterator<ATWACheckpoint> checkpointIt(GetWorld()); checkpointIt; ++checkpointIt)
		{
			if (ATWACheckpoint* checkpoint = (*checkpointIt))
			{
				if (EncompassesPoint(checkpoint->GetActorLocation(), 100.0f))
				{
					Checkpoint = checkpoint;
					break;
				}
			}
		}
	}
}

void ACameraLimitVolume::EndPlay(EEndPlayReason::Type endPlayReason)
{
	if (UBrushComponent* brushComp = GetBrushComponent())
	{
		brushComp->OnComponentBeginOverlap.RemoveDynamic(this, &ACameraLimitVolume::OnCameraLimitBeginOverlap);
		brushComp->OnComponentEndOverlap.RemoveDynamic(this, &ACameraLimitVolume::OnCameraLimitEndOverlap);
	}

	Super::EndPlay(endPlayReason);
}

void ACameraLimitVolume::TriggerOverlapEvents()
{
	if (UBrushComponent* brushComp = GetBrushComponent())
	{
		TSet<AActor*> overllapingActors;
		brushComp->GetOverlappingActors(overllapingActors);

		for (AActor* actor : overllapingActors)
		{
			OnCameraLimitBeginOverlap(brushComp, actor, Cast<UPrimitiveComponent>(actor->GetRootComponent()), 0, false, FHitResult());
		}
	}
}

bool ACameraLimitVolume::IsInsideLimits(const FVector& viewTargetLocation) const
{
	FBox boundBox = GetBounds().GetBox();
	FVector relativeLocation = Vec2D(viewTargetLocation - boundBox.GetCenter());

	FVector extent = boundBox.GetExtent();

	if (relativeLocation.X < -extent.X + ForwardAxisLimit.X || relativeLocation.X > extent.X - ForwardAxisLimit.Y
		|| relativeLocation.Y < -extent.Y + SideAxisLimit.X || relativeLocation.Y > extent.Y - SideAxisLimit.Y)
	{
		return false;
	}

	return true;
}

void ACameraLimitVolume::ClampViewTargetLocation(FVector& inout_ViewTargetLocation) const
{
	FBox boundBox = GetBounds().GetBox();
	FVector volumeCenter = boundBox.GetCenter();
	FVector relativeLocation = Vec2D(inout_ViewTargetLocation - boundBox.GetCenter());

	FVector extent = boundBox.GetExtent();
	if (relativeLocation.X < -extent.X + ForwardAxisLimit.X)
	{
		inout_ViewTargetLocation.X = volumeCenter.X + -extent.X + ForwardAxisLimit.X;
	}
	else if (relativeLocation.X > extent.X - ForwardAxisLimit.Y)
	{
		inout_ViewTargetLocation.X = volumeCenter.X + extent.X - ForwardAxisLimit.Y;
	}

	if(relativeLocation.Y < -extent.Y + SideAxisLimit.X)
	{
		inout_ViewTargetLocation.Y = volumeCenter.Y - extent.Y + SideAxisLimit.X;
	}
	else if(relativeLocation.Y > extent.Y - SideAxisLimit.Y)
	{
		inout_ViewTargetLocation.Y = volumeCenter.Y + extent.Y - SideAxisLimit.Y;
	}
}

void ACameraLimitVolume::SetIgnoreActorWhenMoving(AActor* actor, bool bShouldIgnore)
{
	for (AActor* blockingActor : LevelBlockingActors)
	{
		blockingActor->SetActorEnableCollision(!bShouldIgnore);
	}
}

void ACameraLimitVolume::OnCameraLimitBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (bExited)
	{
		return;
	}

	if (otherActor != nullptr && otherActor->ActorHasTag(TEXT("CameraViewTarget")))
	{
		if (ATWAPawn* pawn = Utils::GetPawn())
		{
			pawn->RecomputeViewTargets();

			for (AActor* viewTarget : pawn->ViewTargets)
			{
				SetIgnoreActorWhenMoving(viewTarget, true);
			}
		}
	}
}

void ACameraLimitVolume::OnCameraLimitEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (otherActor != nullptr && otherActor->ActorHasTag(TEXT("CameraViewTarget")))
	{
		if(ATWAPawn* pawn = Utils::GetPawn())
		{
			pawn->RecomputeViewTargets();

			for (AActor* viewTarget : pawn->ViewTargets)
			{
				SetIgnoreActorWhenMoving(viewTarget, false);
			}
		}

		bExited = true;
	}

}