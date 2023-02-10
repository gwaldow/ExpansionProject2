// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TWAGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GGJ_API ATWAGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> TumbleweedActorClass;

	//////////////////////////////////////////////////////////////////////////
	// Dynamic
	UPROPERTY(transient)
	TArray<UObject*> WindableList;

	UPROPERTY(transient)
	TArray<UObject*> ResetableList;

	UPROPERTY(transient)
	TArray<UObject*> WindClutter;
};
