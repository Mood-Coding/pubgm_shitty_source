﻿#include "pch.h"

#include "Aimbot.h"

Aimbot* g_pAim = new Aimbot();

void Aimbot::MoveMouse(long x, long y)
{
	INPUT Input = { 0 };
	Input.type = INPUT_MOUSE;

	Input.mi.dx = x;
	Input.mi.dy = y;

	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

	SendInput(1, &Input, sizeof(INPUT));
}

void Aimbot::FindBestTarget(Character* bestTarget)
{
	SDK::FVector2D posToCheck{};
	if (Settings::Aimbot::targetBone == BONE_HEAD)
		posToCheck = bestTarget->BONE_HEAD;

	float distToCross{ Utils::DistBetween2Vector2D(posToCheck, SDK::FVector2D(static_cast<float>(g_pD3D->screenW) / 2.0f, static_cast<float>(g_pD3D->screenH) / 2))};

	if (distToCross < tmpNearestDist2Cross)
	{
		tmpNearestDist2Cross = distToCross;
		tmpCharacter = *bestTarget;
	}
}

SDK::FVector2D Aimbot::PredictMovement(SDK::FVector PawnBoneGamePos, SDK::FVector TargetBoneGamePos)
{
	if (PawnBulletFireSpeed > 0.0f)
	{
		// Distance in game position between: Pawn head bone vs Enemy bone
		float distance{ Utils::DistBetween2Vector3D(PawnBoneGamePos, TargetBoneGamePos) };
		// If distance is higher than 100000 so there is an error in tmpCharacter.GAME_BONE_HEAD or PawnHeadBoneGamePos
		if (distance < 80000)
		{
			// Time for bullet to reach the enemy position: t = S / v
			float BulletTravelTime{ distance / PawnBulletFireSpeed };

			// Get enemy velocity: v
			DWORD SceneComponent{ g_pMM->read<DWORD>(g_pAim->tmpCharacter.Address + 0x14C) };
			SDK::FVector ComponentVelocity{ g_pMM->read<SDK::FVector>(SceneComponent + 0x1B0) };

			// The distance that enemy moved 
			SDK::FVector PredictEnemyBonePos{ g_pAim->tmpCharacter.GAME_BONE_HEAD };
			PredictEnemyBonePos.X += ComponentVelocity.X * BulletTravelTime; // S = v * t
			PredictEnemyBonePos.Y += ComponentVelocity.Y * BulletTravelTime; // S = v * t
			PredictEnemyBonePos.Z += ComponentVelocity.Z * BulletTravelTime; // S = v * t

			// Get predicted enemy bone position on screen
			// Don't need to check the return value of this function (is behind my player)
			// because it's already checked in the players for loop
			g_pVMM->GameToScreenBone(PredictEnemyBonePos, g_pAim->tmpTargetPos);

			// Line to best target
//			g_pD3D->DrawLine(g_pD3D->screenW / 2, g_pD3D->screenH, g_pAim->tmpCharacter.PositionOnSc.X, g_pAim->tmpCharacter.PositionOnSc.Y + g_pAim->tmpCharacter.PositionOnSc.Z + yOffset, RED(255));

			// Enemy predict movement line
	//		g_pD3D->DrawLine(g_pAim->tmpTargetPos.X, g_pAim->tmpTargetPos.Y, g_pAim->tmpCharacter.BONE_HEAD.X, g_pAim->tmpCharacter.BONE_HEAD.Y, WHITE(255), 1.5);

			// Dot at the end of enemy movement prediction line
			//g_pD3D->DrawCircle(g_pAim->tmpTargetPos.X, g_pAim->tmpTargetPos.Y, (g_pAim->tmpCharacter.PositionOnSc.Z / 2) / 7, GRAY(255));
		}
		/*float BulletDrop(float TravelTime) {
			return (TravelTime * TravelTime * 980 / 2);
		}*/
		//Class: ShootWeaponEntity.WeaponEntity.WeaponLogicBaseComponent.ActorComponent.Object
	}

	return SDK::FVector2D(0, 0);
}

void Aimbot::GetTmpBestTarget()
{
	targetPos = tmpTargetPos;
	character = tmpCharacter;
}

void Aimbot::ResetTmpNearestTargetDist2Cross()
{
	tmpNearestDist2Cross = 9999.0f;
	tmpTargetPos.X = 0.0f;
	tmpTargetPos.Y = 0.0f;
}

void AimbotLoop(bool* g_bActive)
{
	float half_width{}, half_height{};

	while (g_bActive)
	{
		if (!(GetAsyncKeyState(VK_XBUTTON2) & 0x8000))
		{
			half_width = static_cast<float>(g_pD3D->screenW) / 2;
			half_height = static_cast<float>(g_pD3D->screenH) / 2;

			Sleep(1);
			continue;
		}

//		std::cout << g_pAim->targetPos.X << ' ' << g_pAim->targetPos.Y << '\n';

		float aimX{ 0.0f };
		float aimY{ 0.0f };

		if (g_pAim->targetPos.X != 0.0f)
		{
			if (g_pAim->targetPos.X > half_width)
			{
				// TODO dao nguoc hang tu
				aimX = -(half_width - g_pAim->targetPos.X);
				aimX /= Settings::Aimbot::sensitivity;

				if (aimX + half_width > half_width * 2)
					aimX = 0.0f;
			}
			else if (g_pAim->targetPos.X < half_width)
			{
				aimX = g_pAim->targetPos.X - half_width;
				aimX /= Settings::Aimbot::sensitivity;

				if (aimX + half_width < 0.0f)
					aimX = 0.0f;
			}

			// Avoid shaking when very near to aim position
			if (abs(aimX) < 1.0f)
				aimX = 0.0f;
		}

		if (g_pAim->targetPos.Y != 0.0f)
		{
			if (g_pAim->targetPos.Y > half_height)
			{
				aimY = -(half_height - g_pAim->targetPos.Y);
				aimY /= Settings::Aimbot::sensitivity;

				if (aimY + half_height > half_height * 2)
					aimY = 0.0f;
			}
			else if (g_pAim->targetPos.Y < half_height)
			{
				aimY = g_pAim->targetPos.Y - half_height;
				aimY /= Settings::Aimbot::sensitivity;

				if (aimY + half_height < 0.0f)
					aimY = 0.0f;
			}

			// Avoid shaking when very near to aim position
			if (abs(aimY) < 1.0f)
				aimY = 0.0f;
		}

		// TODO FOV check

		if (aimX == 0.0f && aimY == 0.0f)
			continue;
		
		mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(aimX), static_cast<DWORD>(aimY), 0UL, NULL);

		std::this_thread::sleep_for(std::chrono::milliseconds(Settings::Aimbot::delayBetweenEveryAimbotTime));
	}
}