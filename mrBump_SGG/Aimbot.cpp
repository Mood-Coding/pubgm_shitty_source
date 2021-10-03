#include "pch.h"
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

void Aimbot::FindBestTarget(Character* character)
{
	SDK::FVector2D posToCheck{};
	if (Settings::Aimbot::targetBone == BONE_HEAD)
		posToCheck = character->BONE_HEAD;

	float distToCross{ Utils::DistBetween2Vector2D(posToCheck, SDK::FVector2D(static_cast<float>(g_pD3D->screenW) / 2.0f, static_cast<float>(g_pD3D->screenH) / 2))};

	if (distToCross < tmpNearestDist2Cross)
	{
		tmpNearestDist2Cross = distToCross;
		tmpCharacter = *character;
	}
}

void Aimbot::GetTmpBestTarget()
{
	targetPos = tmpTargetPos;
	character = tmpCharacter;
}

void Aimbot::ResetTmpNearestTargetDist2Cross()
{
	tmpNearestDist2Cross = 9999.0f;
}

void AimbotLoop(bool* g_bActive)
{
	while (g_bActive)
	{
		if (!(GetAsyncKeyState(VK_XBUTTON2) & 0x8000))
		{
			Sleep(1);
			continue;
		}

		float half_width{ static_cast<float>(g_pD3D->screenW) / 2 };
		float half_height{ static_cast<float>(g_pD3D->screenH) / 2 };
		float aimX{ 0.0f };
		float aimY{ 0.0f };

		if (g_pAim->targetPos.X != 0.0f)
		{
			if (g_pAim->targetPos.X > half_width)
			{
				aimX = -(half_width - g_pAim->targetPos.X);
				aimX /= Settings::Aimbot::sensitivity;

				if (aimX + half_width > half_width * 2)
					aimX = 0.0f;
			}

			if (g_pAim->targetPos.X < half_width)
			{
				aimX = g_pAim->targetPos.X - half_width;
				aimX /= Settings::Aimbot::sensitivity;

				if (aimX + half_width < 0.0f)
					aimX = 0.0f;
			}
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

			if (g_pAim->targetPos.Y < half_height)
			{
				aimY = g_pAim->targetPos.Y - half_height;
				aimY /= Settings::Aimbot::sensitivity;

				if (aimY + (float)half_height < 0.0f)
					aimY = 0.0f;
			}
		}

		// FOV check

		// Avoid shaking when very near to aim position
		if (abs(aimX) < 1.0f)
			aimX = 0.0f;
		if (abs(aimY) < 1.0f)
			aimY = 0.0f;

		if (aimX == 0.0f && aimY == 0.0f)
			continue;

		mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(aimX), static_cast<DWORD>(aimY), 0UL, NULL);
	
			/*if (Program.AimOptions.bDrawLock == 1)
			{
				gfx.DrawCircle(FlatSDKInternal.IRenderer._white, x, y, 5f, 5f);
			}*/

		/*Sleep(5);*/
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}