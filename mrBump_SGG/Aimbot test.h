#pragma once
#include "stdafx.h"
#include "Utils.h"
#include "Entity.h"
#include "Overlay.h"

#ifndef AIMBOT_H
#define AIMBOT_H

class Aimbot {
public:
	Aimbot();
	~Aimbot();

	bool Init();
	void FindBestTarget(Entity* curTarget);
	void ResetBestTarget();
	bool IsVisible(DWORD localPlayerAddr, DWORD targetPlayerAddr);

	Vector2f bestTargetPos;
	float bestDistToCrosshair = 8888.f;
	DWORD bestTargetAddr = 0;
	DWORD bestTargetZ = 0;
	std::map<std::string, Vector2f> bestTargetBone{
		{"BONE_HEAD", Vector2f()},
		{"BONE_CHEST", Vector2f()},
		{"BONE_PELVIS", Vector2f()},
	};

	Vector2f middleScreenPos;
	Vector2f aimPos;

	bool bLMouse = false;

	//sẹt tin
	bool bActive = true;
	std::string aimTarget = "BONE_PELVIS";
	int aimFOV = 100;
	float aimSen = 13;
	
};

DWORD WINAPI MOVEMOUSE(LPVOID p);

extern Aimbot* g_pAimbot;

namespace AimbotSettings {
	extern bool bActive;
	extern std::string aimTarget;
}

#endif