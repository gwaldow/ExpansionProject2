#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "TWABlueprintLibrary.generated.h"

//////////////////////////////////////////////////////////////////////////

UCLASS()
class GGJ_API UTWABlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	static class ATWAPawn* GetLocalPawn();

	UFUNCTION(BlueprintPure)
	static class ATWAController* GetLocalController();

	UFUNCTION(BlueprintPure)
	static float GetRemainingBoostPercentage();

	UFUNCTION(BlueprintPure)
	static float GetBoostCooldownPercentage();
	
	UFUNCTION(BlueprintCallable)
	static void StartGame();

	UFUNCTION(BlueprintCallable, Category = "Windable", meta=(DefaultToSelf = "selfObject"))
	static void RegisterWindable(UObject* selfObject);

	UFUNCTION(BlueprintCallable, Category = "Windable", meta = (DefaultToSelf = "selfObject"))
	static void UnregisterWindable(UObject* selfObject);

	UFUNCTION(BlueprintCallable, Category = "Windable", meta = (DefaultToSelf = "selfObject"))
	static void SimplePhysicsWind(UObject* selfObject, const FVector& windForce, float clampForce = -1.0f);

	UFUNCTION(BlueprintCallable, Category = "Resetable", meta = (DefaultToSelf = "selfObject"))
	static void RegisterResetable(UObject* selfObject);

	UFUNCTION(BlueprintCallable, Category = "Resetable", meta = (DefaultToSelf = "selfObject"))
	static void UnregisterResetable(UObject* selfObject);

	UFUNCTION(BlueprintCallable, Category = "Fire", meta = (DefaultToSelf = "selfObject"))
	static bool DefaultIsFlaming(UObject* selfObject);

	UFUNCTION(BlueprintPure)
	static FBoxSphereBounds GetVolumeBounds(AVolume* volume);

	UFUNCTION(BlueprintPure)
	static float GetWindAngle();

	UFUNCTION(BlueprintPure)
	static FVector2D GetWindInput();

	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "actor"))
	static void KillVelocity(AActor* actor);
};