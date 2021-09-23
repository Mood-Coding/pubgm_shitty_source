#pragma once
#include "pch.h"

#include "mrBump_SGG.h"
#include "SDK_classes.h"
#include "SDK_structs.h"
#include "ESP.h"

// Aimbot need PlayerESP toggle is on
// So read mem loop will get player datas for aimbot to find target
class Aimbot
{
public:
	std::vector<Character> Characters;
	float nearestDistToCross = 9999.0f;
	SDK::FVector2D targetPos{};
	DWORD targetAddr = 0;

	bool Init();

	//void AimbotLoop();

	void MoveMouse(long x,long y);

	void FindBestTarget(Character* character);
	
	void ResetTargetValue();
};

extern Aimbot* g_pAim;