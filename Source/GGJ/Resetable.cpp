
#include "Resetable.h"

#include "TWAGameModeBase.h"
#include "TWAUtilities.h"

void UResetable::RegisterResetable(UObject* resetable)
{
	if (ATWAGameModeBase* gameMode = Utils::GetGameMode())
	{
		gameMode->ResetableList.Add(resetable);
	}
}

void UResetable::UnregisterResetable(UObject* resetable)
{
	if (ATWAGameModeBase* gameMode = Utils::GetGameMode())
	{
		gameMode->ResetableList.Remove(resetable);
	}
}