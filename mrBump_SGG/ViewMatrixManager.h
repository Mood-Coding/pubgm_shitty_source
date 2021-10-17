#pragma once

#include "pch.h"
#include "datatype.h"
#include "MemoryManager.h"
#include "D3D.h"
#include "SDK_structs.h"
#include "ESP.h"

#ifndef _VIEWMATRIXMANAGER_H_
#define _VIEWMATRIXMANAGER_H_

class ViewMatrixManager {
public:
	ViewMatrixManager() { };

	D3DMatrix viewMatrix;

	void GetViewMatrix();

	bool WorldToScreen(const SDK::FVector& pos, SDK::FVector& screen, int& distance);
	bool WorldToScreenPlayer(const SDK::FVector& pos, SDK::FVector& screen, int& distance);

	bool GameToScreenBone(const SDK::FVector& pos, SDK::FVector2D& screen);
	SDK::FVector GetBoneGamePosition(const DWORD& bodyAddr, const DWORD& boneAddr);
	D3DMatrix ToMatrixWithScale(const Vector3f& translation, const Vector3f& scale, const Vector4f& rot);
	D3DMatrix MatrixMultiplication(const D3DMatrix& pM1, const D3DMatrix& pM2);

	DWORD GetViewWorld(std::wstring emulator);
};

extern ViewMatrixManager* g_pVMM;

#endif // !_VIEWMATRIXMANAGER_H_


