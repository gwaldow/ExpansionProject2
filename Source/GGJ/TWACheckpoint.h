#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TWACheckpoint.generated.h"

/**
 * 
 */
UCLASS()
class GGJ_API ATWACheckpoint : public AActor
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	USceneComponent* RespawnLocation;

	ATWACheckpoint();

	void Respawn();
};
