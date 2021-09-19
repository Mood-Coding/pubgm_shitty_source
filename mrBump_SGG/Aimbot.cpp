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

	float distToCross = Utils::DistBetween2Vector2D(posToCheck, SDK::FVector2D(g_pD3D->screenW / 2, g_pD3D->screenH / 2));

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
	ZeroMemory(&targetPos, sizeof(targetPos));
}

void AimbotLoop()
{
	while (g_bActive)
	{
		/*for (auto character : g_pAim->Characters)
		{
			g_pESP->GetPlayerBonePos(&character);
			g_pAim->FindBestTarget(&character);
		}*/

		//std::cout << g_pAim->targetPos.X << " " << g_pAim->targetPos.Y << "\n";

		if ((GetAsyncKeyState(VK_XBUTTON2) & 0x8000) && (g_pAim->targetPos.X > 0 && g_pAim->targetPos.Y > 0))
		{
			long posX = static_cast<long>(g_pAim->targetPos.X) - g_pD3D->screenW / 2;
			long posY = static_cast<long>(g_pAim->targetPos.Y) - g_pD3D->screenH / 2;

			//std::cout << posX << " " << posY << "\n";
			g_pAim->MoveMouse(posX, posY);
			std::cout << "aimming" << "\n";
		}

		Sleep(10);
	}

	
}

bool Aimbot::Init()
{
	Characters.reserve(101);

	std::thread aimBot(AimbotLoop);
	// When this function is out of scope, this function won't crash
	aimBot.detach();

	return 1;
}