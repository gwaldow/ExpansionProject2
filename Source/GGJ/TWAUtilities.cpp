
#include "TWAUtilities.h"

#include "Engine/Engine.h"
#include "GameFramework/HUD.h"
#include "GameFramework/WorldSettings.h"

#include "TWAGameModeBase.h"
#include "TWAController.h"
#include "TWAPawn.h"

#if WITH_EDITOR
#include "Editor.h"
#include "UnrealEdGlobals.h"
#include "GameMapsSettings.h"
#endif

UWorld* Utils::GetGameWorld()
{
	UWorld* theWorld = nullptr;

	if (GEngine != nullptr)
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			UWorld* curWorld = Context.World();
			if (curWorld && curWorld->IsGameWorld()) // not sure if this is enough?
			{
#if WITH_EDITOR
				if (GIsEditor && GEditor->PlayWorld == curWorld)
				{
					return curWorld;
				}
#endif

				if (theWorld != nullptr)
				{
					return theWorld;
				}

				theWorld = curWorld;
			}
		}
	}
	return theWorld;
}

UWorld* Utils::GetEditorWorld()
{
	UWorld* theWorld = nullptr;

	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		UWorld* curWorld = Context.World();
		if (curWorld && curWorld->WorldType == EWorldType::Editor)
		{
#if WITH_EDITOR
			if (GIsEditor && GEditor->PlayWorld == curWorld)
			{
				return curWorld;
			}
#endif


			theWorld = curWorld;
		}
	}

	return theWorld;
}

bool Utils::IsPlayInEditor(const UObject* worldContext)
{
	if (IsRunningCommandlet())
	{
		return false; // not editor, but not the game either - not sure what we want in this context
	}

	UWorld* world = worldContext->GetWorld();

	if (!world)
	{
		return true;
	}

	switch (world->WorldType)
	{
	case EWorldType::PIE:
		return true;
	}

	return false;
}

bool Utils::IsInEditor(const UObject* worldContext, bool bAllowEditorPreview)
{
#if !WITH_EDITOR
	return false;
#else
	if (IsRunningCommandlet())
	{
		return false; // not editor, but not the game either - not sure what we want in this context
	}

	UWorld* world = worldContext->GetWorld();

	if (!world)
	{
		return GIsEditor;
	}

	switch (world->WorldType)
	{
	case EWorldType::Editor:
		return true;
	case EWorldType::EditorPreview:
		return bAllowEditorPreview;
	case EWorldType::PIE:
		return false;
	}

	return GIsEditor;
#endif
}

bool Utils::IsInGame(const UObject* worldContext)
{
	if (IsRunningCommandlet() || worldContext == nullptr)
	{
		return false;
	}

	UWorld* world = worldContext->GetWorld();

	if (!world)
	{
		return !GIsEditor;
	}

	switch (world->WorldType)
	{
	case EWorldType::Editor:
	case EWorldType::EditorPreview:
		return false;
	case EWorldType::PIE:
	case EWorldType::Game:
		return true;
	}

	return !GIsEditor;
}

ATWAController* Utils::GetPlayerController()
{
	UWorld* theWorld = GetGameWorld();
	if (theWorld == nullptr) // happens e.g. for cases downstack of OnUnregister() called from blueprint compile
	{
		return nullptr;
	}

	for (FConstPlayerControllerIterator Iterator = theWorld->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ATWAController* playerController = Cast<ATWAController>(Iterator->Get());
		if (playerController && playerController->IsLocalController())
		{
			return playerController;
		}
	}

	return nullptr;
}

ATWAPawn* Utils::GetPawn()
{
	if(ATWAController* controller = GetPlayerController())
	{
		return Cast<ATWAPawn>(controller->GetPawn());
	}

	return nullptr;
}

class AHUD* Utils::GetHUD()
{
	if (ATWAController* controller = GetPlayerController())
	{
		return Cast<AHUD>(controller->GetHUD());
	}

	return nullptr;
}

ATWAGameModeBase* Utils::GetGameMode()
{
	UWorld* world = GetGameWorld();
	if (world != nullptr)
	{
		return Cast<ATWAGameModeBase>(world->GetAuthGameMode());
	}
	else
	{
		return nullptr;
	}
}

/*
ATWAGameModeBase* Utils::GetEditorGameMode()
{
#if WITH_EDITOR
	TSubclassOf<AGameModeBase> gameModeClass = nullptr;
	if (UWorld* editorWorld = GetEditorWorld())
	{
		if (AWorldSettings* worldSettings = editorWorld->GetWorldSettings())
		{
			if (worldSettings->DefaultGameMode != nullptr)
			{
				gameModeClass = worldSettings->DefaultGameMode;
			}
		}
	}

	if (gameModeClass == nullptr)
	{
		UGameMapsSettings* GameMapsSettings = Cast<UGameMapsSettings>(UGameMapsSettings::StaticClass()->GetDefaultObject());

		FSoftClassPath classPath = GameMapsSettings->GetGlobalDefaultGameMode();
		gameModeClass = classPath.TryLoadClass<AGameModeBase>();
	}

	if (gameModeClass != nullptr)
	{
		return Cast<ATWAGameModeBase>(gameModeClass->GetDefaultObject<AGameModeBase>());
	}
#endif

	return nullptr;
}
*/

AWorldSettings* Utils::GetWorldSettings()
{
	UWorld* world = GetGameWorld();
	if (world == nullptr)
	{
		return nullptr;
	}

	return Cast<AWorldSettings>(world->GetWorldSettings());
}

AWorldSettings* Utils::GetEditorWorldSettings()
{
	UWorld* world = GetEditorWorld();
	if (world != nullptr)
	{
		return Cast<AWorldSettings>(world->GetWorldSettings());
	}
	else
	{
		return nullptr;
	}
}

float Utils::GetGameTime()
{
	UWorld* world = Utils::GetGameWorld();

	if (world)
	{
		return world->GetTimeSeconds();
	}

	return 0.0f;
}

float Utils::ElapsedTime(float eventTime)
{
	if (eventTime <= 0.0f)
	{
		return BIG_NUMBER;
	}

	return GetGameWorld()->GetTimeSeconds() - eventTime;
}

bool Utils::IsRecentEvent(float eventTime, float maxDelay)
{
	if (eventTime <= 0.0f)
	{
		return false;
	}

	float elapsedTime = ElapsedTime(eventTime);
	return elapsedTime <= maxDelay;
}

AActor* Utils::GetActorFromObject(UObject* object)
{
	if (AActor* actor = Cast<AActor>(object))
	{
		return actor;
	}
	else if (UActorComponent* component = Cast<UActorComponent>(object))
	{
		return component->GetOwner();
	}

	return nullptr;
}

const AActor* Utils::GetActorFromObject(const UObject* object)
{
	if (const AActor* actor = Cast<AActor>(object))
	{
		return actor;
	}
	else if (const UActorComponent* component = Cast<UActorComponent>(object))
	{
		return component->GetOwner();
	}

	return nullptr;
}


APlayerCameraManager* Utils::GetCameraManager()
{
	APlayerController* playerController = GetPlayerController();
	if (playerController != nullptr)
	{
		return Cast<APlayerCameraManager>(playerController->PlayerCameraManager);
	}

	return nullptr;
}