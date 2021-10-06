#pragma once
#include "pch.h"

#include "SDK_classes.h"
#include "SDK_structs.h"
#include "ESP.h"

void AimbotLoop(bool* g_bActive);

// Aimbot need PlayerESP toggle is on
// So read mem loop will get player datas for aimbot to find target
class Aimbot
{
public:
	//float nearestDist2Cross = 9999.0f;
	SDK::FVector2D targetPos{};
	Character character;

	float tmpNearestDist2Cross = 9999.0f;
	SDK::FVector2D tmpTargetPos{};
	Character tmpCharacter;

	float PawnBulletFireSpeed = 0.0f;
	
	const float GRAVITY = 588.6f;

	void MoveMouse(long x,long y);

	void FindBestTarget(Character* character);

	SDK::FVector2D PredictMovement(SDK::FVector PawnBoneGamePos, SDK::FVector TargetBoneGamePos);

	void GetTmpBestTarget();
	
	void ResetTmpNearestTargetDist2Cross();
};

extern Aimbot* g_pAim;