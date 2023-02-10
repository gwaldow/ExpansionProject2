#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Windable.generated.h"

class IWindObjectCPP
{
public:
	
};

UINTERFACE(BlueprintType)
class GGJ_API UWindable : public UInterface
{
	GENERATED_BODY()

public:
	static void RegisterWindable(UObject* windable);
	static void UnregisterWindable(UObject* windable);

	static void SimplePhysicsWind(UObject* windable, const FVector& windForce, float clampForce);
};

class GGJ_API IWindable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void Wind(const FVector& windForce);
	virtual void Wind_Implementation(const FVector& windForce)
	{
		// Maybe implemented
	}

	UFUNCTION(BlueprintNativeEvent)
	FVector2D GetZLimit() const;
	virtual FVector2D GetZLimit_Implementation() const
	{
		return FVector2D(-1.0f, -1.0f);
	}
};