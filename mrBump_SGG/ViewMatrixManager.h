#pragma once

#include "pch.h"
#include "datatype.h"
#include "MemoryManager.h"
#include "Entity.h"
#include "D3D.h"
#include "SDK_structs.h"
#include "ESP.h"

#ifndef _VIEWMATRIXMANAGER_H_
#define _VIEWMATRIXMANAGER_H_

class ViewMatrixManager {
public:
	ViewMatrixManager() { };

	D3DMatrix viewMatrix;

	bool WorldToScreen(SDK::FVector pos, SDK::FVector& screen, int& distance);
	//bool WorldToScreenPlayer(Vector3f pos, Vector3f& screen, int& distance);
	bool WorldToScreenPlayer(SDK::FVector pos, SDK::FVector& screen, int& distance);
	void WorldToScreenBone(Vector3f pos, SDK::FVector2D& screen);

	Vector3f GetBoneWorldPosition(DWORD bodyAddr, DWORD boneAddr);
	D3DMatrix ToMatrixWithScale(Vector3f translation, Vector3f scale, Vector4f rot);
	D3DMatrix MatrixMultiplication(D3DMatrix pM1, D3DMatrix pM2);
	DWORD GetViewWorld(std::wstring emulator);
	void GetViewMatrix();
};

extern ViewMatrixManager* g_pVMM;

#endif // !_VIEWMATRIXMANAGER_H_


