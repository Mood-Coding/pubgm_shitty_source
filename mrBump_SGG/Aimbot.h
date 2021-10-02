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
	float nearestDist2Cross = 9999.0f;
	SDK::FVector2D targetPos{};
	Character character;

	float tmpNearestDist2Cross = 0.0f;
	SDK::FVector2D tmpTargetPos{};
	Character tmpCharacter;

	void MoveMouse(long x,long y);

	void FindBestTarget(Character* character);

	void GetTmpBestTarget();
	
	void ResetTmpTarget();
};

extern Aimbot* g_pAim;