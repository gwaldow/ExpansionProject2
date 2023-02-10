#pragma once

#include "CoreMinimal.h"

#include "Resetable.generated.h"

UINTERFACE(BlueprintType)
class GGJ_API UResetable : public UInterface
{
	GENERATED_BODY()
public:
	static void RegisterResetable(UObject* resetable);
	static void UnregisterResetable(UObject* resetable);
};

class GGJ_API IResetable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void Reset();
	virtual void Reset_Implementation()
	{
		// Maybe implemented
	}
};