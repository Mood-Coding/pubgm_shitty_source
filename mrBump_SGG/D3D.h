#pragma once
#include "pch.h"

#include "Utils.h"
#include "Colors.h"
#include "Settings.h"
#include "ProcessManager.h"

#pragma warning( push )
#pragma warning( disable : 4820)
#pragma warning( disable : 4514)
#include "ImGui\\imgui.h"
#include "ImGui\\imgui_impl_dx9.h"
#include "ImGui\\imgui_impl_win32.h"
#pragma warning( pop ) 

#define M_PI 3.14159265358979323846264338327950288419716939937510

void EventManager(bool* bActive, bool* g_bDoneReadMem);

template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

class D3D
{
private:
	LPDIRECT3D9 pD3D = nullptr;
	LPD3DXLINE pD3DLine = nullptr;
	LPD3DXSPRITE pSprite = nullptr;
	WNDCLASSEX wc{};

	ImDrawList* imGuiBackgroundDrawList;

public:
	D3D() {}

	HWND emulatorHWND = NULL;
	HWND overlayHWND = NULL;
	int screenW = 0, screenH = 0;

	ImFont* font;

	//std::map<byte, LPD3DXFONT> pFontSimSun;
	LPD3DXFONT pPlayerNameFont;

	LPDIRECT3DDEVICE9 pD3DDevice;

	RECT gameScreenRct{};

	bool SetupHWND();
	bool InitD3D();
	
	void GetImGuiBackgroundDrawList();

	void DrawLine(float x1, float y1, float x2, float y2, unsigned int color, float thickness = 1);

	void DrawRect(int x, int y, int width, int height, unsigned int color, float rounding = 0.0F, float thickness = 1.0F);
	void DrawRect(ImVec2 topleft, ImVec2 botright, unsigned int color, float rounding = 0.0F, float thickness = 1.0F);

	void DrawFilledRect(int x, int y, float width, float height, unsigned int rectColor, unsigned int filledColor);
	void DrawFilled(float x, float y, float width, float height, unsigned int color);

	void DrawCrossX(float x1, float y1, float x2, float y2, D3DCOLOR color);

	void DrawString(int x, int y, unsigned int color, std::wstring& txt, bool bShadow);
	void DrawString(int x, int y, unsigned int color, std::string& txt, bool bShadow);
	void DrawString(float x, float y, unsigned int color, std::string& txt, bool bShadow);
	void DrawString(float x, float y, unsigned int color, const char* txt, bool bShadow);

	void DrawCircle(int x, int y, int radius, unsigned int Color);

	RECT calcTextRect(LPD3DXFONT pFont, std::string txt);

	void CleanupDeviceD3D();

	void MenuTheme();
	void MenuRender();

	void HandleWindow();
	void HandleKeyInput(bool* bActive);

	void TestDraw();
};

extern D3D* g_pD3D;


