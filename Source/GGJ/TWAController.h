#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "TWAController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBoostStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBoostEnded, float, usedBoost);

/**
 * 
 */
UCLASS()
class GGJ_API ATWAController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category="TWA")
	float BaseWindForce = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "TWA")
	float BoostApproch = 0.999f;

	UPROPERTY(EditDefaultsOnly, Category = "TWA")
	float WindAngleApproach = 0.999f;

	UPROPERTY(EditDefaultsOnly, Category = "TWA")
	float WindSpeedApproach = 0.999f;

	UPROPERTY(EditAnywhere, Category = "Boost")
	class UCurveFloat* BoostCurveMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boost")
	float MinimumReboostInterval = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Boost")
	float BoostRegenDuration = 5.0f;

	UPROPERTY(BlueprintAssignable)
	FOnBoostStarted OnBoostStarted;

	UPROPERTY(BlueprintAssignable)
	FOnBoostEnded OnBoostEnded;
	
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	virtual void Tick(float deltaTime) override;

	// Inputs Functions
	UFUNCTION()
	void WindForward(float value);

	UFUNCTION()
	void WindRight(float value);

	UFUNCTION()
	void BoostWindPressed();

	UFUNCTION()
	void BoostWindReleased();

	UFUNCTION(BlueprintCallable)
	FBox GetWindClutterLimits() const;

	UFUNCTION(BlueprintCallable)
	void SnapClutterToGround();

	FVector2D WindInput2D;
	bool bWindWantsBoost;
	float WindBoostStartedTime;
	float WindBoostBurnedOutDuration = 0.0f;

	float WindAngle;
	float WindSpeed;
};
