#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TWAPawn.generated.h"

/**
 * 
 */
UCLASS()
class GGJ_API ATWAPawn : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float CameraTargetLocationApproach = 0.9f;

	UPROPERTY(EditDefaultsOnly)
	float RespawnDelay = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float DeadFadeOutDelay = 3.0f;

	UPROPERTY(Transient)
	TArray<AActor*> ViewTargets;

	UPROPERTY(Transient)
	class ACameraLimitVolume* LevelCameraLimit;

	UPROPERTY(Transient)
	TArray<class ACameraLimitVolume*> CameraLimits;
	
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float deltaTime) override;
	
	void StartGame();
	
	FVector GetTargetViewLocation() const;

	UFUNCTION(BlueprintCallable)
	void RecomputeViewTargets();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeathFadeOut();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerDead();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerRespawn();

	UFUNCTION(BlueprintImplementableEvent)
	void OnCheckpointReached();

	UFUNCTION(BlueprintCallable)
	void OnFireStateChanged();

	bool bGameStarted = false;
	bool bEverHadTargets = false;
	float PlayerDeadTimestamp = 0.0f;
	bool bTriggeredFadeOut = false;

};
