#include "pch.h"

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

bool D3D::SetupHWND()
{
	std::cout << "[D3D]\n";

	if (g_pMM->emuProcName == L"AndroidProcess.exe")
	{
		// Find Smartgaga HWND
		emulatorHWND = FindWindow(L"TitanRenderWindowClass", NULL);
		emulatorHWND = FindWindowEx(emulatorHWND, 0, L"TitanOpenglWindowClass", NULL);
	}
	else if (g_pMM->emuProcName == L"aow_exe.exe")
	{
		// Find Gameloop HWND
		emulatorHWND = FindWindow(L"TXGuiFoundation", L"Gameloop");
		emulatorHWND = FindWindowEx(emulatorHWND, NULL, L"AEngineRenderWindowClass", L"AEngineRenderWindow");
	}

	if (emulatorHWND)
	{
		std::cout << "Emulator HWND: " << std::hex << emulatorHWND << '\n';
	}
	else
	{
		std::cout << "<!> Invalid HWND. Can't find emulator window\n";
		return false;
	}

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
		
		// First font will be used by menu text
		io.Fonts->AddFontFromFileTTF("msyhl.ttf", 20.0f);

		// Second font is for ESP text
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

///////////////////
// DRAW FUNCTION //
///////////////////

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

void D3D::DrawRect(const ImVec2& topleft, const ImVec2& botright, unsigned int color, float rounding, float thickness)
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

void D3D::DrawString(const float& x, const float& y, unsigned int color, const std::string& txt, float size, bool bShadow)
{
	if (bShadow)
	{
		imGuiBackgroundDrawList->AddText(font, size, ImVec2(x + 1.0f, y + 1.0f), BLACK(100), txt.c_str(), NULL, 0.0f, NULL);
		imGuiBackgroundDrawList->AddText(font, size, ImVec2(x + 1.0f, y - 1.0f), BLACK(100), txt.c_str(), NULL, 0.0f, NULL);
	}

	imGuiBackgroundDrawList->AddText(font, size, ImVec2(x, y), color, txt.c_str(), NULL, 0.0f, NULL);
}

void D3D::DrawString(const float& x, const float& y, unsigned int color, const char* txt, float size, bool bShadow)
{
	if (bShadow)
	{
		imGuiBackgroundDrawList->AddText(font, size, ImVec2(x + 1.0f, y + 1.0f), BLACK(100), txt, NULL, 0.0f, NULL);
		imGuiBackgroundDrawList->AddText(font, size, ImVec2(x + 1.0f, y - 1.0f), BLACK(100), txt, NULL, 0.0f, NULL);
	}

	imGuiBackgroundDrawList->AddText(font, size, ImVec2(x, y), color, txt, NULL, 0.0f, NULL);
}

void D3D::DrawString(const float& x, const float& y, unsigned int color, std::wstring& txt, bool bShadow)
{
	RECT rect{};

	if (bShadow)
	{
		rect = { (LONG)x + 1, (LONG)y + 1, (LONG)x + 80, (LONG)y + 50 };
		pPlayerNameFont->DrawText(NULL, txt.c_str(), -1, &rect, DT_NOCLIP | DT_LEFT, BLACK(170));
		rect = { (LONG)x + 1, (LONG)y - 1, (LONG)x + 80, (LONG)y + 50 };
		pPlayerNameFont->DrawText(NULL, txt.c_str(), -1, &rect, DT_NOCLIP | DT_LEFT, BLACK(170)); 
	}
	
	rect = { (LONG)x, (LONG)y, (LONG)x + 80, (LONG)y + 50 };
	pPlayerNameFont->DrawTextW(NULL, txt.c_str(), -1, &rect, DT_NOCLIP | DT_LEFT, color);
}

///////////////////////
// END DRAW FUNCTION //
///////////////////////

inline void LoadConfig()
{
	// Create a file instance
	mINI::INIFile file("config.ini");

	// Create a data structure
	mINI::INIStructure ini;

	bool readSuccess = file.read(ini);

	if (!readSuccess)
	{
		std::cout << "<Read> Can't read config file!\n";
		return;
	}

	// Player
	{
		Settings::PlayerESP::bToggle	 = std::stoi(ini.get("playeresp").get("btoggle"));

		Settings::PlayerESP::bBoneToggle = std::stoi(ini.get("playeresp").get("bbonetoggle"));

		Settings::PlayerESP::bLineToggle = std::stoi(ini.get("playeresp").get("blinetoggle"));

		Settings::PlayerESP::bDistance   = std::stoi(ini.get("playeresp").get("bdistance"));
		Settings::PlayerESP::bName		 = std::stoi(ini.get("playeresp").get("bname"));
		Settings::PlayerESP::bHp		 = std::stoi(ini.get("playeresp").get("bhp"));
	}
	
	//Vehicle
	{
		Settings::VehicleESP::bToggle      = std::stoi(ini.get("vehicleesp").get("btoggle"));

		Settings::VehicleESP::bDistance	   = std::stoi(ini.get("vehicleesp").get("bdistance"));
		Settings::VehicleESP::bFuel		   = std::stoi(ini.get("vehicleesp").get("bfuel"));
		Settings::VehicleESP::bHp          = std::stoi(ini.get("vehicleesp").get("bhp"));
		Settings::VehicleESP::bName        = std::stoi(ini.get("vehicleesp").get("bname"));

		Settings::VehicleESP::drawDistance = std::stoi(ini.get("vehicleesp").get("drawdistance"));
	}

	// Item
	{
		Settings::ItemESP::bToggle      = std::stoi(ini.get("itemesp").get("btoggle"));

		Settings::ItemESP::bDistance    = std::stoi(ini.get("itemesp").get("bdistance"));
		Settings::ItemESP::bName	    = std::stoi(ini.get("itemesp").get("bname"));

		Settings::ItemESP::drawDistance = std::stoi(ini.get("itemesp").get("drawdistance"));
	}

	// Lootbox
	{
		Settings::LootboxESP::bToggle	   = std::stoi(ini.get("lootboxesp").get("btoggle"));

		Settings::LootboxESP::drawDistance = std::stoi(ini.get("lootboxesp").get("drawdistance"));
	}

	// AirDrop
	{
		Settings::AirDropESP::bToggle = std::stoi(ini.get("airdropesp").get("btoggle"));
	}

	// Aimbot
	{
		Settings::Aimbot::bToggle					  = std::stoi(ini.get("aimbot").get("btoggle"));

		Settings::Aimbot::sensitivity				  = std::stoi(ini.get("aimbot").get("sensitivity"));
		Settings::Aimbot::delayBetweenEveryAimbotTime = std::stoi(ini.get("aimbot").get("delaybetweeneveryaimbottime"));
		Settings::Aimbot::fov						  = std::stoi(ini.get("aimbot").get("fov"));
		Settings::Aimbot::selectedBone                = std::stoi(ini.get("aimbot").get("selectedbone"));
		 
		// Upper code just update the bone target on the menu
		// So I have to update the targetBone in Settings too
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

	// Others
	{
		Settings::readMemloopDelay  = std::stoi(ini["other"]["readmemloopdelay"]);
		Settings::drawLoopDelay     = std::stoi(ini["other"]["drawloopdelay"]);
		Settings::bToggleShadowText = std::stoi(ini["other"]["btoggleshadowtext"]);
	}

	std::cout << "Loaded config\n";
}

inline void SaveConfig()
{
	// Create a file instance
	mINI::INIFile file("config.ini");

	// Create a data structure
	mINI::INIStructure ini;

	// Player
	{
		// Toggle
		ini["playeresp"]["btoggle"] = std::to_string(Settings::PlayerESP::bToggle);

		// Bone
		ini["playeresp"]["bbonetoggle"] = std::to_string(Settings::PlayerESP::bBoneToggle);

		// Line
		ini["playeresp"]["blinetoggle"] = std::to_string(Settings::PlayerESP::bLineToggle);

		// Others
		ini["playeresp"]["bdistance"] = std::to_string(Settings::PlayerESP::bDistance);
		ini["playeresp"]["bname"] = std::to_string(Settings::PlayerESP::bName);
		ini["playeresp"]["bhp"] = std::to_string(Settings::PlayerESP::bHp);
	}

	// Vehicle
	{
		ini["vehicleesp"]["btoggle"]	  =	std::to_string(Settings::VehicleESP::bToggle);

		ini["vehicleesp"]["bdistance"]    =	std::to_string(Settings::VehicleESP::bDistance);
		ini["vehicleesp"]["bfuel"]        =	std::to_string(Settings::VehicleESP::bFuel);
		ini["vehicleesp"]["bhp"]          =	std::to_string(Settings::VehicleESP::bHp);
		ini["vehicleesp"]["bname"]        =	std::to_string(Settings::VehicleESP::bName);

		ini["vehicleesp"]["drawdistance"] =	std::to_string(Settings::VehicleESP::drawDistance);
	}

	// Items
	{
		ini["itemesp"]["btoggle"]	   = std::to_string(Settings::ItemESP::bToggle);

		ini["itemesp"]["bdistance"]	   = std::to_string(Settings::ItemESP::bDistance);
		ini["itemesp"]["bname"]		   = std::to_string(Settings::ItemESP::bName);

		ini["itemesp"]["drawdistance"] = std::to_string(Settings::ItemESP::drawDistance);
	}

	// Lootbox
	{
		ini["lootboxesp"]["btoggle"]	  = std::to_string(Settings::LootboxESP::bToggle);

		ini["lootboxesp"]["drawdistance"] = std::to_string(Settings::LootboxESP::drawDistance);
	}

	// AirDrop
	{
		ini["airdropesp"]["btoggle"] = std::to_string(Settings::AirDropESP::bToggle);
	}

	// Aimbot
	{
		ini["aimbot"]["btoggle"]					 = std::to_string(Settings::Aimbot::bToggle);

		ini["aimbot"]["selectedbone"]				 = std::to_string(Settings::Aimbot::selectedBone);
		ini["aimbot"]["sensitivity"]				 = std::to_string(Settings::Aimbot::sensitivity);
		ini["aimbot"]["delaybetweeneveryaimbottime"] = std::to_string(Settings::Aimbot::delayBetweenEveryAimbotTime);
		ini["aimbot"]["fov"]						 = std::to_string(Settings::Aimbot::fov);
	}

	// Other settings
	{
		ini["other"]["readmemloopdelay"]  = std::to_string(Settings::readMemloopDelay);
		ini["other"]["drawloopdelay"]	  = std::to_string(Settings::drawLoopDelay);
		ini["other"]["btoggleshadowtext"] = std::to_string(Settings::bToggleShadowText);
	}

	// Generate an INI file (overwrites any previous file)
	if (file.generate(ini, true))
		std::cout << "Saved config!\n";
	else
		std::cout << "<Generate> Can't create config!\n";
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
	// Main panel
	{
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false);

		// Left panel
		{
			ImGui::BeginChild("left panel", ImVec2(150, 0), true);

			ImU32 activeBtnColor = IM_COL32(47, 47, 60, 255);
			ImU32 inactiveBtnColor = IM_COL32(21, 21, 21, 255);

			ImGui::PushStyleColor(ImGuiCol_Button, Settings::activeTabID == ESP_TAB_ID ? activeBtnColor : inactiveBtnColor);
			if (ImGui::Button("ESP", ImVec2(150 - 9, 41)))
				Settings::activeTabID = ESP_TAB_ID;

			ImGui::PushStyleColor(ImGuiCol_Button, Settings::activeTabID == AIMBOT_TAB_ID ? activeBtnColor : inactiveBtnColor);
			if (ImGui::Button("Aimbot", ImVec2(150 - 9, 41)))
				Settings::activeTabID = AIMBOT_TAB_ID;

			ImGui::PushStyleColor(ImGuiCol_Button, Settings::activeTabID == SETTINGS_TAB_ID ? activeBtnColor : inactiveBtnColor);
			if (ImGui::Button("Settings", ImVec2(150 - 9, 41)))
				Settings::activeTabID = SETTINGS_TAB_ID;

			ImGui::PushStyleColor(ImGuiCol_Button, Settings::activeTabID == DEBUG_TAB_ID ? activeBtnColor : inactiveBtnColor);
			if (ImGui::Button("Debug", ImVec2(150 - 9, 41)))
				Settings::activeTabID = DEBUG_TAB_ID;

			ImGui::PopStyleColor(4);

			ImGui::EndChild();
		}

		ImGui::SameLine();

		// Right panel
		{
			ImGui::BeginGroup();
			ImGui::BeginChild("right panel", ImVec2(0, 0));

			if (Settings::activeTabID == ESP_TAB_ID)
			{
				if (ImGui::CollapsingHeader("Player ESP"))
				{
					ImGui::BeginGroup();

					ImGui::Checkbox("Player ESP Toggle", &Settings::PlayerESP::bToggle);

					ImGui::LabelText("", "Player Information");
					ImGui::Checkbox("Distance", &Settings::PlayerESP::bDistance);
					ImGui::SameLine();
					ImGui::Checkbox("Hp", &Settings::PlayerESP::bHp);
					ImGui::SameLine();
					ImGui::Checkbox("Name", &Settings::PlayerESP::bName);

					ImGui::LabelText("", "Bone ESP");
					ImGui::Checkbox("Toggle bone", &Settings::PlayerESP::bBoneToggle);

					ImGui::LabelText("", "Line ESP");
					ImGui::Checkbox("Toggle line", &Settings::PlayerESP::bLineToggle);

					ImGui::EndGroup();
				}

				if (ImGui::CollapsingHeader("Vehicle ESP"))
				{
					ImGui::BeginGroup();

					ImGui::Checkbox("Vehicle ESP Toggle", &Settings::VehicleESP::bToggle);

					ImGui::Checkbox("Distance ", &Settings::VehicleESP::bDistance);
					ImGui::SameLine();
					ImGui::Checkbox("Hp ", &Settings::VehicleESP::bHp);
					ImGui::SameLine();
					ImGui::Checkbox("Name ", &Settings::VehicleESP::bName);

					ImGui::SliderInt("Draw distance", &Settings::VehicleESP::drawDistance, 1, 1000);

					ImGui::EndGroup();
				}

				if (ImGui::CollapsingHeader("Item ESP"))
				{
					ImGui::BeginGroup();

					ImGui::Checkbox("Toggle Item ESP", &Settings::ItemESP::bToggle);
					
					ImGui::Checkbox("Distance  ", &Settings::ItemESP::bDistance);
					ImGui::SameLine();
					ImGui::Checkbox("Name  ", &Settings::ItemESP::bName);

					ImGui::SliderInt("Drawdistance  ", &Settings::ItemESP::drawDistance, 1, 100);

					ImGui::EndGroup();
				}

				if (ImGui::CollapsingHeader("AirDrop ESP"))
				{
					ImGui::BeginGroup();

					ImGui::Checkbox("Toggle AirDrop ESP", &Settings::AirDropESP::bToggle);

					ImGui::EndGroup();
				}

				if (ImGui::CollapsingHeader("LootBox ESP"))
				{
					ImGui::BeginGroup();

					ImGui::Checkbox("Toggle LootBox ESP", &Settings::LootboxESP::bToggle);

					ImGui::SliderInt("Draw distance   ", &Settings::LootboxESP::drawDistance, 1, 200);

					ImGui::EndGroup();
				}
			}

			if (Settings::activeTabID == AIMBOT_TAB_ID)
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

				ImGui::SliderInt("FOV", &Settings::Aimbot::fov, 1, 180);
			}

			if (Settings::activeTabID == SETTINGS_TAB_ID)
			{
				ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

				ImGui::SliderInt("Draw loop delay", &Settings::drawLoopDelay, 0, 40);

				ImGui::SliderInt("Read mem delay", &Settings::readMemloopDelay, 0, 100);

				ImGui::Text("Read mem loop delay = Read mem delay + Draw loop delay");
				ImGui::Text("= %d", Settings::drawLoopDelay + Settings::readMemloopDelay);

				ImGui::Checkbox("Toogle global text shadow effect", &Settings::bToggleShadowText);
			}

			if (Settings::activeTabID == DEBUG_TAB_ID)
			{
				ImGui::Checkbox("Debug ESP", &Settings::bDebugESP);

				ImGui::Checkbox("ESP unsorted actor", &Settings::bUnsortedActorESP);

				ImGui::Checkbox("Self ESP", &Settings::bSelfESP);

				ImGui::Text("Cached %d actorID <-> actorName", g_pESP->ActorNameCache.size());
			}

			ImGui::EndChild();
			ImGui::EndGroup();
		}

		ImGui::EndChild();
	}
	
	ImGui::Separator();

	// Foot bar
	{
		if (ImGui::Button("Save config", ImVec2(100, 0)))
			SaveConfig();

		ImGui::SameLine();

		if (ImGui::Button("Load config", ImVec2(100, 0)))
			LoadConfig();
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
	if (g_pMM->emuProcName == L"aow_exe.exe")
	{
		emulatorHWND = FindWindow(L"TXGuiFoundation", L"Gameloop");
		emulatorHWND = FindWindowEx(emulatorHWND, NULL, L"AEngineRenderWindowClass", L"AEngineRenderWindow");
	}
	else
	{
		emulatorHWND = FindWindow(L"TitanRenderWindowClass", NULL);
		emulatorHWND = FindWindowEx(emulatorHWND, 0, L"TitanOpenglWindowClass", NULL);
	}

	GetWindowRect(emulatorHWND, &g_pD3D->gameScreenRct);

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