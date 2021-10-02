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
	DWORD targetAddr = 0;

	float tmpNearestDist2Cross = 0.0f;
	SDK::FVector2D tmpTargetPos{};
	DWORD tmpTargetAddr = 0;

	void MoveMouse(long x,long y);

	void FindBestTarget(Character* character);
	
	void ResetTarget();
};

extern Aimbot* g_pAim;