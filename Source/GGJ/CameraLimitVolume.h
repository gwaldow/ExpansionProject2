#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"

#include "CameraLimitVolume.generated.h"

/**
 * 
 */
UCLASS()
class GGJ_API ACameraLimitVolume : public AVolume
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FVector2D ForwardAxisLimit = FVector2D(400, 750.0f);

	UPROPERTY(EditAnywhere)
	FVector2D SideAxisLimit = FVector2D(1000.0f, 1000.0f);

	UPROPERTY(EditAnywhere)
	TArray<AActor*> LevelBlockingActors;

	UPROPERTY(EditAnywhere)
	class ATWACheckpoint* Checkpoint;

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type endPlayReason) override;

	void TriggerOverlapEvents();

	bool IsInsideLimits(const FVector& viewTargetLocation) const;
	void ClampViewTargetLocation(FVector& inout_ViewTargetLocation) const;

	void SetIgnoreActorWhenMoving(AActor* actor, bool bShouldIgnore);

	UFUNCTION()
	void OnCameraLimitBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
	void OnCameraLimitEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);


	bool bExited = false;
};
