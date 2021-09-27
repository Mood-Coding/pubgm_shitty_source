#include "pch.h"
#include "ViewMatrixManager.h"
//#include "Offsets.h"

ViewMatrixManager* g_pVMM = new ViewMatrixManager();

bool ViewMatrixManager::WorldToScreen(SDK::FVector pos, SDK::FVector &screen, int& distance) {
	float ScreenW = (viewMatrix._14 * pos.X) + (viewMatrix._24 * pos.Y) + (viewMatrix._34 * pos.Z + viewMatrix._44);

	//entity is behind player
	if (ScreenW < 0.0001f)
		return false;

	distance = static_cast<int>(std::round(ScreenW / 100));
	
	float ScreenX = (viewMatrix._11 * pos.X) + (viewMatrix._21 * pos.Y) + (viewMatrix._31 * pos.Z + viewMatrix._41);
	float ScreenY = (viewMatrix._12 * pos.X) + (viewMatrix._22 * pos.Y) + (viewMatrix._32 * pos.Z + viewMatrix._42);

	screen.X = (g_pD3D->screenW / 2) + (g_pD3D->screenW / 2) * ScreenX / ScreenW;
	screen.Y = (g_pD3D->screenH / 2) - (g_pD3D->screenH / 2) * ScreenY / ScreenW;
	return true;
}

//bool ViewMatrixManager::WorldToScreenPlayer(Vector3f pos, Vector3f& screen, int& distance) {
bool ViewMatrixManager::WorldToScreenPlayer(SDK::FVector pos, SDK::FVector& screen, int& distance)
{
	float screenW = (viewMatrix._14 * pos.X) + (viewMatrix._24 * pos.Y) + (viewMatrix._34 * pos.Z + viewMatrix._44);
	//entity is behind player
	if (screenW < 0.0001f)
		return false;

	distance = static_cast<int>(std::round(screenW / 100));
	
	float screenX = (viewMatrix._11 * pos.X) + (viewMatrix._21 * pos.Y) + (viewMatrix._31 * pos.Z + viewMatrix._41);
	float screenY = (viewMatrix._12 * pos.X) + (viewMatrix._22 * pos.Y) + (viewMatrix._32 * (pos.Z + 85) + viewMatrix._42);
	float y1 = (g_pD3D->screenH / 2) - (viewMatrix._12 * pos.X + viewMatrix._22 * pos.Y + viewMatrix._32 * (pos.Z - 95) + viewMatrix._42) * (g_pD3D->screenH / 2) / screenW;

	screen.X = (g_pD3D->screenW / 2) + (g_pD3D->screenW / 2) * screenX / screenW;
	screen.Y = (g_pD3D->screenH / 2) - (g_pD3D->screenH / 2) * screenY / screenW;
	screen.Z = y1 - screen.Y;
	return true;
}

Vector3f ViewMatrixManager::GetBoneWorldPosition(DWORD bodyAddr, DWORD boneAddr)
{
	FTransform body = g_pMM->ftRead(bodyAddr);
	FTransform bone = g_pMM->ftRead(boneAddr);
	D3DMatrix boneMatrix = ToMatrixWithScale(bone.Translation, bone.Scale3D, bone.Rotation);
	D3DMatrix componentToWorldMatrix = ToMatrixWithScale(body.Translation, body.Scale3D, body.Rotation);
	D3DMatrix newMatrix = MatrixMultiplication(boneMatrix, componentToWorldMatrix);

	Vector3f boneWorldPosition;
	boneWorldPosition.x = newMatrix._41;
	boneWorldPosition.y = newMatrix._42;
	boneWorldPosition.z = newMatrix._43;

	return boneWorldPosition;
}

D3DMatrix ViewMatrixManager::ToMatrixWithScale(Vector3f translation, Vector3f scale, Vector4f rot) {
	D3DMatrix m;
	m._41 = translation.x;
	m._42 = translation.y;
	m._43 = translation.z;

	float x2 = rot.x + rot.x;
	float y2 = rot.y + rot.y;
	float z2 = rot.z + rot.z;

	float xx2 = rot.x * x2;
	float yy2 = rot.y * y2;
	float zz2 = rot.z * z2;
	m._11 = (1.0f - (yy2 + zz2)) * scale.x;
	m._22 = (1.0f - (xx2 + zz2)) * scale.y;
	m._33 = (1.0f - (xx2 + yy2)) * scale.z;

	float yz2 = rot.y * z2;
	float wx2 = rot.w * x2;
	m._32 = (yz2 - wx2) * scale.z;
	m._23 = (yz2 + wx2) * scale.y;

	float xy2 = rot.x * y2;
	float wz2 = rot.w * z2;
	m._21 = (xy2 - wz2) * scale.y;
	m._12 = (xy2 + wz2) * scale.x;

	float xz2 = rot.x * z2;
	float wy2 = rot.w * y2;
	m._31 = (xz2 + wy2) * scale.z;
	m._13 = (xz2 - wy2) * scale.x;

	m._14 = 0.0f;
	m._24 = 0.0f;
	m._34 = 0.0f;
	m._44 = 1.0f;

	return m;
}

void ViewMatrixManager::WorldToScreenBone(Vector3f pos, SDK::FVector2D& screen)
{
	float screenW{  ( (viewMatrix._14 * pos.x) + (viewMatrix._24 * pos.y) + (viewMatrix._34 * pos.z + viewMatrix._44) )};

	//entity is behind player
	if (screenW < 0.0001f)
		return;

	screenW = 1 / screenW;
	screen.X = (g_pD3D->screenW / 2) + (viewMatrix._11 * pos.x + viewMatrix._21 * pos.y + viewMatrix._31 * pos.z + viewMatrix._41) * screenW * (g_pD3D->screenW / 2);
	screen.Y = (g_pD3D->screenH / 2) - (viewMatrix._12 * pos.x + viewMatrix._22 * pos.y + viewMatrix._32 * pos.z + viewMatrix._42) * screenW * (g_pD3D->screenH / 2);
}

D3DMatrix ViewMatrixManager::MatrixMultiplication(D3DMatrix pM1, D3DMatrix pM2) {
	D3DMatrix pOut =
	{
		pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41,
		pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42,
		pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43,
		pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44,
		pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41,
		pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42,
		pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43,
		pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44,
		pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41,
		pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42,
		pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43,
		pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44,
		pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41,
		pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42,
		pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43,
		pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44
	};

	return pOut;
}

DWORD ViewMatrixManager::GetViewWorld(std::wstring emulator)
{
	BYTE pattern[] = { 0x02, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01 };

	std::vector<DWORD_PTR> foundedBases{};

	// :D

	if (emulator == L"AndroidProcess.exe")
	{
		g_pMM->search(pattern, sizeof(pattern), 0x80000000, 0x90000000, false, 0, foundedBases);

		for (int i = 0; i < foundedBases.size(); i++) {
			DWORD cand = foundedBases[i] - 0x20;
			DWORD eng{ g_pMM->read<DWORD>(cand) + 0x20 };
			DWORD tmp{ g_pMM->read<DWORD>(eng) + 0x200 };
			float v1, v2, v3, v4;

			v1 = g_pMM->read<float>(tmp + 0x8);
			v2 = g_pMM->read<float>(tmp + 0x18);
			v3 = g_pMM->read<float>(tmp + 0x28);
			v4 = g_pMM->read<float>(tmp + 0x38);
			if (v1 == 0 && v2 == 0 && v3 == 0 && v4 >= 3)
			{
				return cand;
			}
		}
	}

	if (emulator == L"aow_exe.exe")
	{
		g_pMM->search(pattern, sizeof(pattern), 0x26000000, 0x30000000, false, 0, foundedBases);
		for (int i = 0; i < foundedBases.size(); i++) {
			DWORD cand = foundedBases[i] - 0x20;
			DWORD eng{ g_pMM->read<DWORD>(cand) + 0x20 };
			DWORD tmp{ g_pMM->read<DWORD>(eng) + 0x200 };
			float v1, v2, v3, v4;

			v1 = g_pMM->read<float>(tmp + 0x8);
			v2 = g_pMM->read<float>(tmp + 0x18);
			v3 = g_pMM->read<float>(tmp + 0x28);
			v4 = g_pMM->read<float>(tmp + 0x38);
			if (v1 == 0 && v2 == 0 && v3 == 0 && v4 >= 3)
			{
				return cand;
			}
		}

		foundedBases.clear();
		g_pMM->search(pattern, sizeof(pattern), 0x30000000, 0x40000000, false, 0, foundedBases);
		for (int i = 0; i < foundedBases.size(); i++) {
			DWORD cand = foundedBases[i] - 0x20;
			DWORD eng{ g_pMM->read<DWORD>(cand) + 0x20 };
			DWORD tmp{ g_pMM->read<DWORD>(eng) + 0x200 };
			float v1, v2, v3, v4;

			v1 = g_pMM->read<float>(tmp + 0x8);
			v2 = g_pMM->read<float>(tmp + 0x18);
			v3 = g_pMM->read<float>(tmp + 0x28);
			v4 = g_pMM->read<float>(tmp + 0x38);
			if (v1 == 0 && v2 == 0 && v3 == 0 && v4 >= 3)
			{
				return cand;
			}
		}

		foundedBases.clear();
		g_pMM->search(pattern, sizeof(pattern), 0x40000000, 0x50000000, false, 0, foundedBases);
		for (int i = 0; i < foundedBases.size(); i++) {
			DWORD cand = foundedBases[i] - 0x20;
			DWORD eng{ g_pMM->read<DWORD>(cand) + 0x20 };
			DWORD tmp{ g_pMM->read<DWORD>(eng) + 0x200 };
			float v1, v2, v3, v4;

			v1 = g_pMM->read<float>(tmp + 0x8);
			v2 = g_pMM->read<float>(tmp + 0x18);
			v3 = g_pMM->read<float>(tmp + 0x28);
			v4 = g_pMM->read<float>(tmp + 0x38);
			if (v1 == 0 && v2 == 0 && v3 == 0 && v4 >= 3)
			{
				return cand;
			}
		}
	}
		
	return 0;
}

void ViewMatrixManager::GetViewMatrix()
{	
	g_pMM->readMemory((PVOID)g_pESP->viewMatrixAddr, &g_pVMM->viewMatrix, 64);
}