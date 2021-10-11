﻿#include "pch.h"

#include "D3D.h"

D3D* g_pD3D = new D3D();

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_PAINT:
		break;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0); // We need to use this to exit a message loop
		break;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool D3D::SetupHWND()
{
	std::cout << "[D3D]\n";

	// Find Smartgaga HWND
	HWND processHWND = FindWindow(L"TitanRenderWindowClass", NULL);
	processHWND = FindWindowEx(processHWND, 0, L"TitanOpenglWindowClass", NULL);
	if (processHWND)
	{
		g_pPM->emuProcessName = L"AndroidProcess.exe";
		goto FOUNDHWND;
	}
	
	// Find Gameloop HWND
	processHWND = FindWindow(L"TXGuiFoundation", L"Gameloop");
	processHWND = FindWindowEx(processHWND, NULL, L"AEngineRenderWindowClass", L"AEngineRenderWindow");
	if (processHWND)
	{
		g_pPM->emuProcessName = L"aow_exe.exe";
		goto FOUNDHWND;
	}

	std::cout << "<!> Invalid HWND. Can't find emulator window\n";
	return false;

FOUNDHWND:

	emulatorHWND = processHWND;
	std::cout << "Target process window HWND: " << std::hex << emulatorHWND << '\n';

	GetWindowRect(emulatorHWND, &gameScreenRct);
	screenW = gameScreenRct.right - gameScreenRct.left;
	screenH = gameScreenRct.bottom - gameScreenRct.top;

	// Create overlay window
	{
		// Random class name, window name
		std::string str = Utils::RandomString(8);
		std::wstring stemp = std::wstring(str.begin(), str.end());
		LPCWSTR sw = stemp.c_str();

		wc = WNDCLASSEX();
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(0, IDI_APPLICATION);
		wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
		wc.lpszMenuName = sw;
		wc.lpszClassName = sw;
		RegisterClassEx(&wc);

		overlayHWND = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, wc.lpszClassName, wc.lpszMenuName, WS_POPUP, gameScreenRct.left, gameScreenRct.top, screenW, screenH, NULL, NULL, NULL, NULL);

		std::cout << "Overlay HWND: " << overlayHWND << std::endl;

		InitD3D();

		SetLayeredWindowAttributes(overlayHWND, 0, 1, LWA_ALPHA);
		SetLayeredWindowAttributes(overlayHWND, 0, RGB(0, 0, 0), LWA_COLORKEY);
		MARGINS margin = { -1, -1, -1, -1 };
		DwmExtendFrameIntoClientArea(overlayHWND, &margin);

		//SetForegroundWindow(overlayHWND);
		ShowWindow(overlayHWND, SW_SHOW);
		UpdateWindow(overlayHWND);
	}

	// Setup Dear ImGui context
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		io = ImGui::GetIO();

		std::ifstream iFile;
		iFile.open("msyhl.ttf");
		if (!iFile)
		{
			std::cout << "<!> Can't find font\n";
			return false;
		}
		//io.Fonts->AddFontFromFileTTF("msyhl.ttf", 17);
		font = io.Fonts->AddFontFromFileTTF("msyhl.ttf", 45.0f);

		ImGuiStyle* style = &ImGui::GetStyle();
		style->Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 255);
		/*style->WindowMinSize = ImVec2(437,371) */

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(overlayHWND);
		ImGui_ImplDX9_Init(pD3DDevice);
	}

	return true;
}

bool D3D::InitD3D()
{
	//create the Direct3D interface
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);    
	if (pD3D == nullptr)
	{
		std::cout << "Direct3DCreate9 failed" << std::endl;
		return 0;
	}

	//create a struct to hold various device information
	D3DPRESENT_PARAMETERS d3dpp;    	
	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = overlayHWND;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;     // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = screenW;    // set the width of the buffer
	d3dpp.BackBufferHeight = screenH;    // set the height of the buffer
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	D3DCAPS9 caps;
	DWORD behaviorFlags;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps))) {
		std::cout << "GetDeviceCaps failed" << std::endl;
		return 0;
	}
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
		behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		std::cout << "BehaviorFlags: D3DCREATE_HARDWARE_VERTEXPROCESSING" << std::endl;
	} else {
		behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		std::cout << "BehaviorFlags: D3DCREATE_SOFTWARE_VERTEXPROCESSING" << std::endl;
	}

	// create a device class using this information and the info from the d3dpp stuct
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, overlayHWND, behaviorFlags, &d3dpp, &pD3DDevice)))
	{
		std::cout << "<D3D> CreateDevice failed!" << std::endl;
		return 0;
	}

	D3DXCreateFont(pD3DDevice, 20, 0, FW_REGULAR, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Microsoft Yahei"), &pPlayerNameFont);

	return 1;
}

void D3D::CleanupDeviceD3D()
{
	SafeRelease(&pD3D);
	SafeRelease(&pD3DDevice);

	::DestroyWindow(overlayHWND);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

void D3D::GetImGuiBackgroundDrawList()
{
	imGuiBackgroundDrawList = ImGui::GetBackgroundDrawList();
}

void D3D::DrawCircle(const float& x, const float& y, const float& radius, const unsigned int& Color)
{
	imGuiBackgroundDrawList->AddCircle(ImVec2(x, y), radius, Color);
}

void D3D::DrawLine(const float& x1, const float& y1, const float& x2, const float& y2, const unsigned int& color, float thickness)
{
	imGuiBackgroundDrawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);
}

void D3D::DrawLine(const ImVec2& pos1, const ImVec2& pos2, const unsigned int& color, float thickness)
{
	imGuiBackgroundDrawList->AddLine(pos1, pos2, color, thickness);
}

void D3D::DrawRect(float x, float y, float width, float height, unsigned int color, float rounding, float thickness)
{
	imGuiBackgroundDrawList->AddRect(ImVec2(x, y), ImVec2(x + width, y + height), color, rounding, 0, thickness);
}

void D3D::DrawRect(ImVec2 topleft, ImVec2 botright, unsigned int color, float rounding, float thickness)
{
	imGuiBackgroundDrawList->AddRect(topleft, botright, color, rounding, 0, thickness);
}

void D3D::DrawCrossX(float x1, float y1, float x2, float y2, D3DCOLOR color)
{
	DrawLine(x1, y1, x2, y2, color);
	DrawLine(x1, y2, x2, y1, color);
}

void D3D::DrawFilled(float x, float y, float width, float height, unsigned int color)
{
	imGuiBackgroundDrawList->AddRectFilled(ImVec2(x, y), ImVec2(x + width, y + height), color);
}

void D3D::DrawFilledRect(float x, float y, float width, float height, unsigned int rectColor, unsigned int filledColor)
{
	DrawFilled(x, y, width, height, filledColor);
	DrawRect(x, y, width, height, rectColor);
}

void D3D::DrawString(const float& x, const float& y, unsigned int color, std::string& txt, float size, bool bShadow)
{
	if (bShadow)
	{
		imGuiBackgroundDrawList->AddText(font, size, ImVec2(x + 1, y + 1), BLACK(200), txt.c_str(), NULL, 0.0f, NULL);
		imGuiBackgroundDrawList->AddText(font, size, ImVec2(x + 1, y - 1), BLACK(200), txt.c_str(), NULL, 0.0f, NULL);
	}

	imGuiBackgroundDrawList->AddText(font, size, ImVec2(x, y), color, txt.c_str(), NULL, 0.0f, NULL);
}

void D3D::DrawString(float x, float y, unsigned int color, const char* txt, float size, bool bShadow)
{
	if (bShadow)
	{
		imGuiBackgroundDrawList->AddText(font, size, ImVec2(x + 1, y + 1), BLACK(200), txt, NULL, 0.0f, NULL);
		imGuiBackgroundDrawList->AddText(font, size, ImVec2(x + 1, y - 1), BLACK(200), txt, NULL, 0.0f, NULL);
	}

	imGuiBackgroundDrawList->AddText(font, size, ImVec2(x, y), color, txt, NULL, 0.0f, NULL);
}

void D3D::DrawString(const float& x, const float& y, unsigned int color, std::wstring& txt, bool bShadow)
{
	if (bShadow)
	{
		RECT rect1{ (LONG)x + 1, (LONG)y + 1, (LONG)x + 80, (LONG)y + 50 };
		pPlayerNameFont->DrawText(NULL, txt.c_str(), -1, &rect1, DT_NOCLIP | DT_LEFT, BLACK(255));
		rect1 = { (LONG)x + 1, (LONG)y - 1, (LONG)x + 80, (LONG)y + 50 };
		pPlayerNameFont->DrawText(NULL, txt.c_str(), -1, &rect1, DT_NOCLIP | DT_LEFT, BLACK(255)); 
	}
	
	RECT rect2 = { (LONG)x, (LONG)y, (LONG)x + 80, (LONG)y + 50 };
	pPlayerNameFont->DrawTextW(NULL, txt.c_str(), -1, &rect2, DT_NOCLIP | DT_LEFT, color);
}


void D3D::MenuTheme()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	/*style->WindowBorderSize = 0;
	style->WindowMinSize = ImVec2(650, 300);*/

	style->WindowBorderSize = 1;
	style->WindowTitleAlign = ImVec2(0.5, 0.5);
	style->WindowPadding = ImVec2(5, 5);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	//style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	/*style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);*/
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

}

void D3D::MenuRender()
{
	ImGui::Columns(2);
	ImGui::SetColumnOffset(1, 160);

	// Left side
	{
		ImU32 activeBtnColor = IM_COL32(47, 47, 60, 255);
		ImU32 inactiveBtnColor = IM_COL32(21, 21, 21, 255);

		ImGui::PushStyleColor(ImGuiCol_Button, Settings::tab == ESP_TAB_ID ? activeBtnColor : inactiveBtnColor);

		if (ImGui::Button("ESP", ImVec2(150 - 15, 41)))
		{
			Settings::tab = ESP_TAB_ID;
		}

		ImGui::PushStyleColor(ImGuiCol_Button, Settings::tab == AIMBOT_TAB_ID ? activeBtnColor : inactiveBtnColor);

		if (ImGui::Button("Aimbot", ImVec2(150 - 15, 41)))
		{
			Settings::tab = AIMBOT_TAB_ID;
		}

		ImGui::PushStyleColor(ImGuiCol_Button, Settings::tab == SETTINGS_TAB_ID ? activeBtnColor : inactiveBtnColor);

		if (ImGui::Button("Settings", ImVec2(150 - 15, 41)))
		{
			Settings::tab = SETTINGS_TAB_ID;
		}

		ImGui::PushStyleColor(ImGuiCol_Button, Settings::tab == DEBUG_TAB_ID ? activeBtnColor : inactiveBtnColor);

		if (ImGui::Button("Debug", ImVec2(150 - 15, 41)))
		{
			Settings::tab = DEBUG_TAB_ID;
		}

		ImGui::PopStyleColor(4);
	}

	ImGui::NextColumn();

	// Right side
	{
		if (Settings::tab == ESP_TAB_ID)
		{
			if (ImGui::CollapsingHeader("Player ESP"))
			{
				ImGui::BeginChild("00", ImVec2(275, 25), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
				ImGui::Checkbox("Toggle", &Settings::PlayerESP::bToggle);
				ImGui::EndChild();

				ImGui::LabelText("", "Player Information");
				ImGui::BeginChild("1", ImVec2(275, 35), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
				ImGui::Checkbox("Distance", &Settings::PlayerESP::bDistance);
				ImGui::SameLine();
				ImGui::Checkbox("Hp", &Settings::PlayerESP::bHp);
				ImGui::SameLine();
				ImGui::Checkbox("Name", &Settings::PlayerESP::bName);
				ImGui::EndChild();

				ImGui::LabelText("", "Bone ESP");
				ImGui::BeginChild("2", ImVec2(275, 35), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
				ImGui::Checkbox("Toggle", &Settings::PlayerESP::BoneESP::bToggle);
				ImGui::EndChild();

				ImGui::LabelText("", "Line ESP");
				ImGui::BeginChild("3", ImVec2(275, 35), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
				ImGui::Checkbox("Toggle", &Settings::PlayerESP::LineESP::bToggle);
				ImGui::EndChild();
			}

			if (ImGui::CollapsingHeader("Vehicle ESP"))
			{
				ImGui::BeginChild("", ImVec2(330, 65), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
				ImGui::Checkbox("Toggle", &Settings::VehicleESP::bToggle);
				ImGui::SameLine();
				ImGui::Checkbox("Distance", &Settings::VehicleESP::bDistance);
				ImGui::SameLine();
				ImGui::Checkbox("Hp", &Settings::VehicleESP::bHp);
				ImGui::SameLine();
				ImGui::Checkbox("Name", &Settings::VehicleESP::bName);
				ImGui::SliderInt("Draw distance", &Settings::VehicleESP::drawDistance, 1, 1000);
				ImGui::EndChild();
			}

			if (ImGui::CollapsingHeader("Item ESP"))
			{
				ImGui::BeginChild("", ImVec2(325, 65), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
				ImGui::Checkbox("Toggle", &Settings::ItemESP::bToggle);
				ImGui::SameLine();
				ImGui::Checkbox("Distance", &Settings::ItemESP::bDistance);
				ImGui::SameLine();
				ImGui::Checkbox("Name", &Settings::ItemESP::bName);
				ImGui::SliderInt("Draw distance", &Settings::ItemESP::drawDistance, 1, 100);
				ImGui::EndChild();
			}

			if (ImGui::CollapsingHeader("AirDrop ESP"))
			{
				ImGui::BeginChild("", ImVec2(325, 65), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
				ImGui::Checkbox("Toggle", &Settings::AirDropESP::bToggle);
				ImGui::EndChild();
			}

			if (ImGui::CollapsingHeader("LootBox ESP"))
			{
				ImGui::BeginChild("", ImVec2(325, 65), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
				ImGui::Checkbox("Toggle", &Settings::LootboxESP::bToggle);
				ImGui::SliderInt("Draw distance", &Settings::LootboxESP::drawDistance, 1, 200);
				ImGui::EndChild();
			}
		}

		if (Settings::tab == AIMBOT_TAB_ID)
		{
			ImGui::Checkbox("Toggle", &Settings::Aimbot::bToggle);
			ImGui::SliderInt("Sensitivity", &Settings::Aimbot::sensitivity, 2, 30);
			ImGui::SliderInt("Delay between every mouse_event time ", &Settings::Aimbot::delayBetweenEveryAimbotTime, 1, 100);
			ImGui::Combo("Bone", &Settings::Aimbot::selectedBone, Settings::Aimbot::bone);

			switch (Settings::Aimbot::selectedBone)
			{
				case(0):
				{
					Settings::Aimbot::targetBone = BONE_HEAD;
					break;
				}
				case(1):
				{
					Settings::Aimbot::targetBone = BONE_CHEST;
					break;
				}
				case(2):
				{
					Settings::Aimbot::targetBone = BONE_PELVIS;
					break;
				}
			}
		}

		if (Settings::tab == SETTINGS_TAB_ID)
		{
			ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::SliderInt("Draw loop delay", &Settings::drawLoopDelay, 0, 40);
			ImGui::SliderInt("Read mem delay", &Settings::readMemloopDelay, 0, 100);
			ImGui::Text("Read mem loop delay = Read mem delay + Draw loop delay");
			ImGui::Text("= %d", Settings::drawLoopDelay + Settings::readMemloopDelay);
			ImGui::Checkbox("Toogle global text shadow effect", &Settings::bToggleShadowText);
		}

		if (Settings::tab == DEBUG_TAB_ID)
		{
			ImGui::Checkbox("Debug ESP", &Settings::bDebugESP);
			ImGui::Checkbox("ESP unsorted actor", &Settings::bUnsortedActorESP);
			ImGui::Checkbox("Self ESP", &Settings::bSelfESP);
			ImGui::Text("Cached %d actorID <-> actorName", g_pESP->ActorNameCache.size());
		}
	}
}

void ChangeClickability(bool canclick, HWND hwnd)
{
	long style = GetWindowLong(hwnd, GWL_EXSTYLE);
	if (canclick)
	{
		style &= ~WS_EX_LAYERED;
		SetWindowLong(hwnd, GWL_EXSTYLE, style);
		SetForegroundWindow(hwnd);
	}
	else
	{
		style |= WS_EX_LAYERED;
		SetWindowLong(hwnd, GWL_EXSTYLE, style);
	}
}

void D3D::HandleWindow()
{
	// When we minimized emulator window, its current hwnd will be invalid
	// So we need to get the new emulator hwnd
	if (g_pPM->emuProcessName == L"aow_exe.exe")
	{
		g_pD3D->emulatorHWND = FindWindow(L"TXGuiFoundation", L"Gameloop");
		g_pD3D->emulatorHWND = FindWindowEx(g_pD3D->emulatorHWND, NULL, L"AEngineRenderWindowClass", L"AEngineRenderWindow");
	}
	else
	{
		g_pD3D->emulatorHWND = FindWindow(L"TitanRenderWindowClass", NULL);
		g_pD3D->emulatorHWND = FindWindowEx(g_pD3D->emulatorHWND, 0, L"TitanOpenglWindowClass", NULL);
	}

	GetWindowRect(g_pD3D->emulatorHWND, &g_pD3D->gameScreenRct);

	// Emulator screen w and h
	g_pD3D->screenW = g_pD3D->gameScreenRct.right - g_pD3D->gameScreenRct.left;
	g_pD3D->screenH = g_pD3D->gameScreenRct.bottom - g_pD3D->gameScreenRct.top;

	SetWindowPos(
		g_pD3D->overlayHWND,
		NULL,
		g_pD3D->gameScreenRct.left,
		g_pD3D->gameScreenRct.top,
		g_pD3D->gameScreenRct.right - g_pD3D->gameScreenRct.left,
		g_pD3D->gameScreenRct.bottom - g_pD3D->gameScreenRct.top,
		3u
	);

	// Move overlay window when we move emulator window
	MoveWindow(
		g_pD3D->overlayHWND, 
		g_pD3D->gameScreenRct.left, 
		g_pD3D->gameScreenRct.top, 
		g_pD3D->gameScreenRct.right - g_pD3D->gameScreenRct.left,
		g_pD3D->gameScreenRct.bottom - g_pD3D->gameScreenRct.top, 
		true
	);

	MARGINS margins{ -1, -1, -1, -1 };
	DwmExtendFrameIntoClientArea(g_pD3D->overlayHWND, &margins);

	// Auto close hack when emulator process is closed
	if (g_pD3D->emulatorHWND == NULL)
		::PostQuitMessage(0);
}

void D3D::HandleKeyInput(bool* bActive)
{
	if (GetAsyncKeyState(VK_HOME) & 1)
	{
		Settings::bShowMenu = !Settings::bShowMenu;
		ChangeClickability(Settings::bShowMenu, g_pD3D->overlayHWND);
		return;
	}

	if (GetAsyncKeyState(VK_END) & 1)
	{
		*bActive = false;
		return;
	}

	if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_CONTROL) & 0x1)
	{
		Settings::bDebugESP = !Settings::bDebugESP;
		return;
	}

	if (GetAsyncKeyState(VK_CAPITAL) & 1)
	{
		Settings::bFightMode = !Settings::bFightMode;
		return;
	}
}

void EventManager(bool* bActive, bool* g_bDoneReadMem)
{
	while (*bActive)
	{
		g_pD3D->HandleWindow();

		g_pD3D->HandleKeyInput(bActive);

		Sleep(20);
	}
}