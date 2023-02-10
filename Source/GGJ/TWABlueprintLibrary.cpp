#include "TWABlueprintLibrary.h"

#include "TWAController.h"
#include "TWAUtilities.h"
#include "Windable.h"
#include "Flammable.h"
#include "GameFramework/Volume.h"
#include "TWAPawn.h"
#include "Resetable.h"

class ATWAPawn* UTWABlueprintLibrary::GetLocalPawn()
{
	return Utils::GetPawn();
}

ATWAController* UTWABlueprintLibrary::GetLocalController()
{
	return Utils::GetPlayerController();
}

float UTWABlueprintLibrary::GetRemainingBoostPercentage()
{
	if (ATWAController* controller = Utils::GetPlayerController())
	{
		if (controller->BoostCurveMultiplier != nullptr)
		{
			float windBoostElapsedTime = Utils::ElapsedTime(controller->WindBoostStartedTime);
			return (controller->BoostCurveMultiplier->FloatCurve.GetLastKey().Time - windBoostElapsedTime)/controller->BoostCurveMultiplier->FloatCurve.GetLastKey().Time;
		}
	}
	
	return 0.0f;
}

float UTWABlueprintLibrary::GetBoostCooldownPercentage()
{
	if (ATWAController* controller = Utils::GetPlayerController())
	{
		return (controller->BoostRegenDuration - controller->WindBoostBurnedOutDuration)/controller->BoostRegenDuration;
	}

	return 0.0f;
}

void UTWABlueprintLibrary::StartGame()
{
	if (ATWAPawn* player = Utils::GetPawn())
	{
		player->StartGame();
	}
}

void UTWABlueprintLibrary::RegisterWindable(UObject* self)
{
	UWindable::RegisterWindable(self);
}

void UTWABlueprintLibrary::UnregisterWindable(UObject* self)
{
	UWindable::UnregisterWindable(self);
}

void UTWABlueprintLibrary::SimplePhysicsWind(UObject* selfObject, const FVector& windForce, float clampForce)
{
	UWindable::SimplePhysicsWind(selfObject, windForce, clampForce);
}

void UTWABlueprintLibrary::RegisterResetable(UObject* selfObject)
{
	UResetable::RegisterResetable(selfObject);
}

void UTWABlueprintLibrary::UnregisterResetable(UObject* selfObject)
{
	UResetable::UnregisterResetable(selfObject);
}

bool UTWABlueprintLibrary::DefaultIsFlaming(UObject* selfObject)
{
	return UFlammable::DefaultIsFlaming(selfObject);
}

FBoxSphereBounds UTWABlueprintLibrary::GetVolumeBounds(AVolume* volume)
{
	return volume->GetBounds();
}

float UTWABlueprintLibrary::GetWindAngle()
{
	if (ATWAController* controller = Utils::GetPlayerController())
	{
		return controller->WindAngle;
	}

	return 0.0f;
}

FVector2D UTWABlueprintLibrary::GetWindInput()
{
	if (ATWAController* controller = Utils::GetPlayerController())
	{
		return controller->WindInput2D;
	}

	return FVector2D();
}


void UTWABlueprintLibrary::KillVelocity(AActor* actor)
{
	if(actor != nullptr)
	{
		if (UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(actor->GetRootComponent()))
		{
			primComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
			primComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			primComp->PutRigidBodyToSleep();
		}
	}
}