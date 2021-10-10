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
	SDK::FVector TargetGamePos;
	SDK::FVector2D TargetScPos;
	SDK::FVector2D TargetScPredictedPos;
	Character TargetCharacter;

	SDK::FVector2D AimPos{};

	float NearestDist2Cross = 9999.0f;

	float PawnBulletFireSpeed{ 0.0f };
	
	const float GRAVITY{ 5.886f };

	void FindBestTarget(Character* character);

	SDK::FVector2D PredictMovement(SDK::FVector PawnBoneGamePos, SDK::FVector TargetBoneGamePos);

	void GetTmpBestTarget();
	
	void ResetTmpNearestTargetDist2Cross();
};

extern Aimbot* g_pAim;