#include "Aimbot.h"


Aimbot* g_pAimbot = new Aimbot();

Aimbot::Aimbot() {

}

Aimbot::~Aimbot() {

}

bool Aimbot::Init() {
	CloseHandle(CreateThread(0, 0, MOVEMOUSE, 0, 0, 0));
	std::cout << "Create aimbot thread" << std::endl;
	return 1;
}



void Aimbot::FindBestTarget(Entity* curTarget) {
	float distToCrosshair = GetDistanceBetween2Points(Vector2f(curTarget->positionOnScreen.x, curTarget->positionOnScreen.y), middleScreenPos);
	if (distToCrosshair < bestDistToCrosshair) {
		bestDistToCrosshair = distToCrosshair;
		bestTargetPos = Vector2f(curTarget->positionOnScreen.x, curTarget->positionOnScreen.y);
		bestTargetAddr = curTarget->entityAddr;
		bestTargetBone[aimTarget] = aimTarget == "BONE_HEAD" ? curTarget->BONE_HEAD : aimTarget == "BONE_CHEST" ? curTarget->BONE_CHEST : curTarget->BONE_PELVIS;
		bestTargetZ = curTarget->positionOnScreen.z;
	}
}

void Aimbot::ResetBestTarget() {
	bestTargetPos = Vector2f();
	bestDistToCrosshair = 8888.f;
	bestTargetZ = 0;
	bestTargetAddr = 0;
}

DWORD WINAPI MOVEMOUSE(LPVOID p) {
	Vector2f aimPos;

	while (true) {
		int aimFOV = g_pAimbot->aimFOV;
		//mouse 5 && pos của target ko đc cùng = 0
		if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000 && !(g_pAimbot->bestTargetPos.x == 0 && g_pAimbot->bestTargetPos.y == 0)) {			
			aimPos.x = g_pAimbot->bestTargetBone[g_pAimbot->aimTarget].x - g_pAimbot->middleScreenPos.x;
			aimPos.y = g_pAimbot->bestTargetBone[g_pAimbot->aimTarget].y - g_pAimbot->middleScreenPos.y;

			if ((aimPos.x > -aimFOV && aimPos.x < aimFOV) && (aimPos.y > -aimFOV && aimPos.y < aimFOV)) {
				//gần mục tiêu thì hạ speed để ko bị lắc
				if ((aimPos.x > -13 && aimPos.x < 13) && (aimPos.y > -13 && aimPos.y < 13)) {
					aimPos.x *= 0.01;
					//aimPos.y *= 0.4;
				}

				//shit avoid recoil system <(")
				if (g_pAimbot->bLMouse) {
					if (g_pAimbot->middleScreenPos.y < g_pAimbot->bestTargetBone[g_pAimbot->aimTarget].y) {
						//float yAssist = GetDistanceBetween2Points(g_pAimbot->bestTargetBone[g_pAimbot->aimTarget], g_pAimbot->middleScreenPos);
						aimPos.y += GetDistanceBetween2Points(g_pAimbot->bestTargetBone[g_pAimbot->aimTarget], g_pAimbot->middleScreenPos);
						//std::cout << yAssist << std::endl;
					}
					
				}
				
				/*std::cout << "x " << aimPos.x << std::endl;
				std::cout << "y " << aimPos.y << std::endl;*/
			

				mouse_event(MOUSEEVENTF_MOVE, aimPos.x / g_pAimbot->aimSen, aimPos.y / g_pAimbot->aimSen, 0, 0);

				if ( ((GetAsyncKeyState(VK_LBUTTON) & 0x8001) == 0x8001) || ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0x8000)) {
					//lần aim sau sẽ kích hoạt hỗ trợ ghìm tâm
					g_pAimbot->bLMouse = true;
				} else {
					//reset lại hỗ trợ ghìm tâm
					g_pAimbot->bLMouse = false;
				}
				
			}
		}
		Sleep(1);
	}
}

bool Aimbot::IsVisible(DWORD localPlayerAddr, DWORD targetPlayerAddr) {
	DWORD localMesh = g_pMemoryManager->dRead(localPlayerAddr + 0x310);
	DWORD targetMesh = g_pMemoryManager->dRead(targetPlayerAddr + 0x310);

	byte bRecentlyRendered = g_pMemoryManager->bRead(targetMesh + 0x64c);

	float localRenderTime = g_pMemoryManager->fRead(localMesh + 0x2fc);
	float targetRenderTime = g_pMemoryManager->fRead(targetMesh + 0x2fc);

	bool visiable1 = false;
	bool visiable = false;
	if (localRenderTime == targetRenderTime) {
		visiable1 = true;
	}
	if (bRecentlyRendered > 0) {
		visiable = true;
	}
	/*std::cout << localPlayerAddr << std::endl;
	std::cout << targetPlayerAddr << std::endl;
	std::cout << localMesh << std::endl;
	std::cout << targetMesh << std::endl;
	std::cout << bRecentlyRendered << std::endl;
	std::cout << localRenderTime << std::endl;
	std::cout << targetRenderTime << std::endl;
	std::cout << visiable << " " << visiable1 << std::endl;
	system("pause");*/
	return visiable == true && visiable1 == true;
}

