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

	//std::cout << character->BONE_HEAD.X << " " << character->BONE_HEAD.Y << '\n';

	float distToCross = Utils::DistBetween2Vector2D(posToCheck, SDK::FVector2D(static_cast<float>(g_pD3D->screenW) / 2.0f, static_cast<float>(g_pD3D->screenH) / 2));

	if (distToCross < nearestDistToCross)
	{
		nearestDistToCross = distToCross;

		targetPos = posToCheck;

		targetAddr = character->Address;
	}
}

void Aimbot::ResetTargetValue()
{
	nearestDistToCross = 9999.0f;
	targetAddr = 0;
	targetPos.X = -999;
	targetPos.Y = -999;
}

void AimbotLoop()
{
	while (g_bActive)
	{
		//std::cout << g_pAim->targetPos.X << " " << g_pAim->targetPos.Y << "\n";

		if ((GetAsyncKeyState(VK_XBUTTON2) & 0x8000) && (g_pAim->targetPos.X > 0 && g_pAim->targetPos.Y > 0))
		{
			long targetX = static_cast<long>(g_pAim->targetPos.X) - g_pD3D->screenW / 2;
			long targetY = static_cast<long>(g_pAim->targetPos.Y) - g_pD3D->screenH / 2;

			//nếu crosshair gần đến điểm aim trên mục tiêu thì hạ tốc độ aimbot
			if ((targetX > -5 && targetX < 5) && (targetY > -5 && targetY < 5)) {
				targetX *= 0.1;
				targetY *= 0.1;
			}
			

			targetX /= Settings::Aimbot::sensitivity;
			targetY /= Settings::Aimbot::sensitivity;

			mouse_event(MOUSEEVENTF_MOVE, (DWORD)(targetX), (DWORD)(targetY), 0, 0);
		}

		Sleep(1);
	}

	
}

bool Aimbot::Init()
{

	//std::thread aimBot(AimbotLoop);

	// When this function is out of scope, this function won't crash
	//aimBot.detach();

	return 1;
}