

#include "TWACheckpoint.h"

#include "EngineUtils.h"

#include "TWAUtilities.h"
#include "TWAController.h"
#include "CameraLimitVolume.h"
#include "TWAGameModeBase.h"

ATWACheckpoint::ATWACheckpoint()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	RespawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("RespawnLocation"));
	RespawnLocation->SetupAttachment(RootComponent);
}

void ATWACheckpoint::Respawn()
{
	if (ATWAGameModeBase* gameMode = Utils::GetGameMode())
	{
		if (gameMode->TumbleweedActorClass != nullptr && RespawnLocation != nullptr)
		{
			FVector Location = RespawnLocation->GetComponentLocation();
			FRotator Rotation(0.0f, 0.0f, 0.0f);
			FActorSpawnParameters SpawnInfo; 
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			GetWorld()->SpawnActor(gameMode->TumbleweedActorClass, &Location, &Rotation, SpawnInfo);
		}
	}
}