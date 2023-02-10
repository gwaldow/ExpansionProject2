
#pragma once

#include "CoreMinimal.h"

//////////////////////////////////////////////////////////////////////////

#if UE_BUILD_SHIPPING
#define TWEAKABLE const
#else
#define TWEAKABLE static
#endif

//////////////////////////////////////////////////////////////////////////

#define COS_0 1.0
#define COS_1 0.9998476952
#define COS_2 0.999390827
#define COS_3 0.9986295348
#define COS_5 0.9961946981
#define COS_10 0.984807753
#define COS_15 0.9659258263
#define COS_20 0.9396926208
#define COS_25 0.9063077870
#define COS_30 0.8660254038
#define COS_35 0.8191520443
#define COS_40 0.7660444431
#define COS_45 0.7071067812
#define COS_50 0.6427876097
#define COS_55 0.5735764363
#define COS_60 0.5
#define COS_70 0.3420201433
#define COS_80 0.1736481777
#define COS_90 0
#define COS_100 -0.1736481777
#define COS_110 -0.3420201433
#define COS_120 -0.5
#define COS_130 -0.6427876097
#define COS_140 -0.7660444431
#define COS_150 -0.8660254038
#define COS_160 -0.9396926208
#define COS_170 -0.984807753
#define COS_180 -1

FORCEINLINE FVector VecZ(float z) { return FVector(0, 0, z); }
FORCEINLINE FVector Vec2D(const FVector& v) { return FVector(v.X, v.Y, 0); }
FORCEINLINE FVector Vec2D(const FVector2D& v) { return FVector(v.X, v.Y, 0); }
FORCEINLINE FVector Vec3D(const FVector2D& v) { return FVector(v.X, v.Y, 0); }
FORCEINLINE FVector2D ToVec2D(const FVector& v) { return FVector2D(v.X, v.Y); }

FORCEINLINE float Saturate(float value)
{
	return FMath::Clamp(value, 0.0f, 1.0f);
}

template< class T, class U > FORCEINLINE U MapClamped(T x, T minX, T maxX, U outEdge0, U outEdge1)
{
	return FMath::Clamp((U)(outEdge0 + (outEdge1 - outEdge0)*(x - minX) / (maxX - minX)), FMath::Min(outEdge0, outEdge1), FMath::Max(outEdge0, outEdge1));
}

template< class T > FORCEINLINE FLinearColor MapClamped(T x, T minX, T maxX, FLinearColor outEdge0, FLinearColor outEdge1)
{
	float ratio = Saturate((x - minX) / (maxX - minX));
	return outEdge0 + ratio*(outEdge1 - outEdge0);
}

FORCEINLINE float Unlerp(float val, float edge0, float edge1) // returns 0.0f if val is past edge 0, 1.0f if past edge1, and lerped in between - same as MapClamped(val, edge0, edge1, 0.0f, 1.0f)
{
	return FMath::Clamp((val - edge0) / (edge1 - edge0), 0.0f, 1.0f);
}

FORCEINLINE float LerpClamped(float alpha, float minVal, float maxVal)
{
	return MapClamped(alpha, 0.0f, 1.0f, minVal, maxVal);
}

//////////////////////////////////////////////////////////////////////////

#define COS_0 1.0
#define COS_1 0.9998476952
#define COS_2 0.999390827
#define COS_3 0.9986295348
#define COS_5 0.9961946981
#define COS_10 0.984807753
#define COS_15 0.9659258263
#define COS_20 0.9396926208
#define COS_25 0.9063077870
#define COS_30 0.8660254038
#define COS_35 0.8191520443
#define COS_40 0.7660444431
#define COS_45 0.7071067812
#define COS_50 0.6427876097
#define COS_55 0.5735764363
#define COS_60 0.5
#define COS_70 0.3420201433
#define COS_80 0.1736481777
#define COS_90 0
#define COS_100 -0.1736481777
#define COS_110 -0.3420201433
#define COS_120 -0.5
#define COS_130 -0.6427876097
#define COS_140 -0.7660444431
#define COS_150 -0.8660254038
#define COS_160 -0.9396926208
#define COS_170 -0.984807753
#define COS_180 -1

//////////////////////////////////////////////////////////////////////////

struct GGJ_API Utils
{
	//
	// Math Functions
	//


	template<class T> FORCEINLINE static T Approach(const T& current, const T& target, float coeff, float deltaTime)
	{
		return target + (current - target) * FMath::Pow((1.0f - coeff), deltaTime);
	}

	FORCEINLINE static float NormalizeRotAngle(float angle)
	{
		angle = FMath::Fmod(angle, 360.0f);

		if (angle <= -180.0f)
		{
			angle += 360.0f;
		}
		else if (angle >= 180.0f)
		{
			angle -= 360.0f;
		}
		checkSlow(angle >= -180.0f && angle <= 180.0f);
		return angle;
	}

	FORCEINLINE static float ApproachAngle(const float current, float target, float coeff, float deltaTime) // Degrees
	{
		return Utils::NormalizeRotAngle(target + Utils::NormalizeRotAngle(current - target) * FMath::Pow((1.0f - coeff), deltaTime));
	}

	//
	// Global Objects Accessors
	//
	
	static class UWorld* GetGameWorld();
	static class UWorld* GetEditorWorld();
	static class ATWAGameModeBase* GetGameMode();
	//static class ATWAGameModeBase* GetEditorGameMode();
	static class AWorldSettings* GetWorldSettings();
	static class AWorldSettings* GetEditorWorldSettings();
	static class APlayerCameraManager* GetCameraManager();	

	//
	// Object Accessor Utilities
	//
	
	static class ATWAController* GetPlayerController();
	static class ATWAPawn* GetPawn();
	static class AHUD* GetHUD();	

	//
	// Generic Queries
	//
	
	static bool IsPlayInEditor(const UObject* worldContext);
	static bool IsInEditor(const UObject* worldContext, bool bAllowEditorPreview = true);	
	static bool IsInGame(const UObject* worldContext); // in a game world, not the menu, but includes lobby

	static AActor* GetActorFromObject(UObject* object);
	static const AActor* GetActorFromObject(const UObject* object);
	static FTransform GetWorldTransformFromObject(const UObject* object);
	
	static float GetGameTime(); // local, starts upon game world start
	static double GetRealTime(); // independent on game state, carries through different maps. Meaningless on its own, must be compared to another.
	static float ElapsedTime(float eventTime);
	static bool IsRecentEvent(float eventTime, float maxDelay); // uses GameTime
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////