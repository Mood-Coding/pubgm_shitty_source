#include "pch.h"

#include "mrBump_SGG.h"

bool g_bDoneReadMem = false;
bool g_bActive = true;

std::vector<Character> tmpCharacters;
int g_tmpCharacterCount = 0;

std::vector<Vehicle> tmpVehicles;

std::vector<Item> tmpItems;

std::vector<Airdrop> tmpAirDrops;
std::vector<BoxData> tmpLootboxes;
std::vector<BoxData> tmpAirDropDatas;

std::vector<UnsortedActor> tmpUnsortedActors;

DWORD tmpViewMatrixAddr = 0;

bool bInGame = false;

void inline AddToCharacters(const std::string& currActorName, const DWORD& currActorAddr, const SDK::FVector& currActorPos)
{
	Character character(currActorAddr, currActorPos);

	if (character.Address == g_pESP->Pawn)
	{
		g_pESP->MyTeamID = g_pMM->read<DWORD>(g_pESP->Pawn + TEAMID);
	}

	// Read whole STExtraCharacter class
	character.STExtraCharacter = g_pMM->read<SDK::STExtraCharacter>(currActorAddr);

	// This character is in my team so skip
	if (character.STExtraCharacter.TeamID == g_pESP->MyTeamID)
		// But it won't skip when current character is my character and SelfESP is on
		if (!(character.Address == g_pESP->Pawn && Settings::bSelfESP))
			return;

	if (Settings::PlayerESP::bName)
		character.PlayerName = g_pESP->GetPlayerName(character.STExtraCharacter.PlayerName);

	// TODO add bDead to struct STExtraCharacter
	bool bDead = g_pMM->read<bool>(character.Address + 0x964);
	if (bDead)
		return;

	// emplace_back a local object is faster than push_back a local object :v
	tmpCharacters.emplace_back(character);
	++g_tmpCharacterCount;
}

void inline AddToVehicles(const std::string& currActorName, const DWORD& currActorAddr, const SDK::FVector& currActorPos)
{
	Vehicle vehicle(currActorAddr, currActorPos);

	if (currActorName != "BP_AirDropPlane_C")
	{
		DWORD VehicleCommonComponent = g_pMM->read<DWORD>(currActorAddr + VEHICLECOMMON);
		vehicle.VehicleCommonComponent = g_pMM->read<SDK::VehicleCommonComponent>(VehicleCommonComponent);
	}

	vehicle.displayName = ActorDisplayName[currActorName];
	if (vehicle.displayName == "")
		vehicle.displayName = currActorName;

	tmpVehicles.emplace_back(vehicle);
}

void inline AddToItems(const std::string& currActorName, const DWORD& currActorAddr, const SDK::FVector& currActorPos)
{
	Item item(currActorAddr, currActorPos);

	item.displayName = ActorDisplayName[currActorName];

	// std::unoreder_map will return empty string if currActorName isn't exist in ActorDisplayName
	if (item.displayName == "")
		item.displayName = currActorName;

	tmpItems.emplace_back(item);
}

void UpdateValue()
{
	while (g_bActive)
	{
		g_pESP->UWorld = g_pMM->read<DWORD>(g_pESP->GWorld);

		// When UWorld == 0, if we read the next value (NetDriver), it will break the hack
		if (g_pESP->UWorld == 0)
			continue;

		DWORD NetDriver = g_pMM->read<DWORD>(g_pESP->UWorld + NETDRIVER);
		
		// If player is in a match, NetDriver != 0 
		if (NetDriver != 0)
		{
			bInGame = true;
		}
		else
		{
			bInGame = false;

			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			continue;
		}

		tmpUnsortedActors.clear();

		tmpCharacters.clear();

		tmpVehicles.clear();

		tmpItems.clear();

		tmpAirDrops.clear();
		tmpAirDropDatas.clear();
		tmpLootboxes.clear();

		g_tmpCharacterCount = 0;

		tmpViewMatrixAddr = g_pMM->read<DWORD>(g_pMM->read<DWORD>(g_pESP->viewWorld) + 32) + 512;
		

		// Get my character address
		DWORD NetConnection = g_pMM->read<DWORD>(NetDriver + SERVERCONNECTION);
		DWORD PlayerController = g_pMM->read<DWORD>(NetConnection + PLAYERCONTROLLER);
		g_pESP->Pawn = g_pMM->read<DWORD>(PlayerController + ACKNOWLEDGEDPAWN);

		// Get ActorList and maxActorCount
		g_pESP->Level = g_pMM->read<DWORD>(g_pESP->UWorld + PERSISTENTLEVEL);
		g_pESP->ActorList = g_pMM->read<DWORD>(g_pESP->Level + g_pESP->ActorListOffset);
		g_pESP->maxActorCount = g_pMM->read<DWORD>(g_pESP->Level + g_pESP->maxActorCountOffset);

		// Match info
		/*DWORD AuthorityGameMode = 0x168;
		DWORD GameModeBase = g_pMM->read<DWORD>(g_pESP->UWorld + 0x168);
		DWORD GameStateBase = g_pMM->read<DWORD>(GameModeBase + 0x2ec);*/

		// Loop through actorlist
		// maxActorCount * 4 : Because size of actor address is 4 byte 
		for (DWORD pActorAddr = g_pESP->ActorList; pActorAddr <= g_pESP->ActorList + g_pESP->maxActorCount * 4; pActorAddr += 4)
		{
			// TODO read array of pActorAddrs then loop through it
			// instead reading every single pActorAddr value
			DWORD currActorAddr = g_pMM->read<DWORD>(pActorAddr);

			if (currActorAddr == NULL)
				continue;

			std::string currActorName{};

			DWORD currActorID{ g_pMM->read<DWORD>(currActorAddr + 0x10) };

			// If current actorID isn't exist in ActorNameCache
			if (g_pESP->ActorNameCache.find(currActorID) == g_pESP->ActorNameCache.end())
			{
				// If not found in ActorNameCache
				// then get actorName by GetActorName func and add current actorName to ActorNameCache
				currActorName = g_pESP->GetActorName(currActorID);
				g_pESP->ActorNameCache.insert( std::make_pair(currActorID, currActorName) );
			}
			else
			{
				currActorName = g_pESP->ActorNameCache[currActorID];
			}

			DWORD SceneComponent{ g_pMM->read<DWORD>(currActorAddr + ROOTCOMPONENT) };
			SDK::FVector currActorPos = g_pMM->read<SDK::FVector>(SceneComponent + ACTORPOSITION);

			if (g_pESP->IsItem(currActorName) && (Settings::ItemESP::bToggle || Settings::bDebugESP))
			{
				Item item(currActorAddr, currActorPos);

				std::string actorDisplayName = ActorDisplayName[currActorName];
				if (actorDisplayName != "")
					item.displayName = actorDisplayName;
				else
					item.displayName = currActorName;

				if (Settings::bDebugESP)
				{
					item.ItemDefineID = g_pMM->read<SDK::ItemDefineID>(item.Address + ITEMDEFINEID);
					item.actorName = currActorName;
				}

				tmpItems.emplace_back(item);
				
				continue;
			}

			if (g_pESP->IsVehicle(currActorName) && (Settings::VehicleESP::bToggle || Settings::bDebugESP))
			{
				AddToVehicles(currActorName, currActorAddr, currActorPos);

				continue;
			}

			if (g_pESP->IsPlayer(currActorName) && (Settings::PlayerESP::bToggle || Settings::bDebugESP))
			{
				AddToCharacters(currActorName, currActorAddr, currActorPos);

				continue;
			}

			if (g_pESP->IsAirdrop(currActorName) && (Settings::AirDropESP::bToggle || Settings::bDebugESP))
			{
				Airdrop airdrop(currActorAddr, currActorPos);

				tmpAirDrops.emplace_back(airdrop);

				continue;
			}

			if (g_pESP->IsAirDropData(currActorName) && (Settings::AirDropESP::bToggle || Settings::bDebugESP))
			{	
				BoxData airDropData{ currActorName, currActorAddr, currActorPos };
				g_pESP->GetBoxItems(&airDropData);

				tmpAirDropDatas.emplace_back(airDropData);

				continue;
			}

			if (g_pESP->IsLootbox(currActorName) && (Settings::LootboxESP::bToggle || Settings::bDebugESP))
			{
				BoxData lootboxData(currActorName, currActorAddr, currActorPos);
				g_pESP->GetBoxItems(&lootboxData);

				tmpLootboxes.emplace_back(lootboxData);

				continue;
			}

			if (Settings::bUnsortedActorESP)
			{
				UnsortedActor unsortedActor(currActorName, currActorPos);

				tmpUnsortedActors.emplace_back(unsortedActor);

				continue;
			}

		}

		g_bDoneReadMem = true;

		// Wait until draw loop take all data from UpdateValue()
		while (g_bDoneReadMem)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(Settings::readMemloopDelay));
	}
}

int main()
{
	// Find Smartgaga HWND
	HWND targetHWND = FindWindow(L"TitanRenderWindowClass", NULL);
	targetHWND = FindWindowEx(targetHWND, 0, L"TitanOpenglWindowClass", NULL);

	if (targetHWND)
	{
		g_pPM->emuProcName = L"AndroidProcess.exe";
	}
	else
	{
		// Find Gameloop HWND
		targetHWND = FindWindow(L"TXGuiFoundation", L"Gameloop");
		targetHWND = FindWindowEx(targetHWND, NULL, L"AEngineRenderWindowClass", L"AEngineRenderWindow");

		g_pPM->emuProcName = L"aow_exe.exe";
	}

    if (!g_pD3D->SetupHWND(targetHWND))
	{
		system("pause");
        return 0;
    }

	if (!g_pPM->Init(g_pPM->emuProcName))
	{
		system("pause");
		return 0;
	}

	if (!g_pMM->Init(g_pPM->hProcess, g_pPM->processPID))
	{
		system("pause");
		return 0;
	}

	if (!g_pESP->Init(g_pPM->emuProcName))
	{
		system("pause");
		return 0;
	}

	if (!g_pAim->Init())
	{
		system("pause");
		return 0;
	}
	
	// Avoid reallocation
	tmpAirDrops.reserve(20);
	tmpAirDropDatas.reserve(20);
	tmpCharacters.reserve(101);
	tmpLootboxes.reserve(101);
	tmpUnsortedActors.reserve(512);
	tmpItems.reserve(200);
	tmpVehicles.reserve(50);

	// Start read memory loop
	std::thread readMem(UpdateValue);

	// TODO handle window, keyinput loop

	// Prepare FPS limiter
	using clock = std::chrono::steady_clock;
	auto next_frame = clock::now();

	// Main draw loop
	while (g_bActive)
	{
		MSG msg;
		// Handle input message
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				ImGui_ImplDX9_Shutdown();
				ImGui_ImplWin32_Shutdown();
				ImGui::DestroyContext();

				g_pD3D->CleanupDeviceD3D();

				// Stop draw loop + read mem loop
				g_bActive = false;

				// Stop read mem loop
				g_bDoneReadMem = false;

				break;
			}
		}

		// TODO create thread for these 2 functions
		g_pD3D->HandleWindow();
		
		g_pD3D->HandleKeyInput();

		// Get data from read mem loop data when it complete reading memory
		if (g_bDoneReadMem)
		{
			g_pESP->UnsortedActors = tmpUnsortedActors;

			g_pESP->Characters = tmpCharacters;
			g_pESP->CharacterCount = g_tmpCharacterCount;

			g_pESP->Items = tmpItems;

			g_pESP->Vehicles = tmpVehicles;

			g_pESP->Lootboxes = tmpLootboxes;
			g_pESP->AirDropDatas = tmpAirDropDatas;
			g_pESP->Airdrops = tmpAirDrops;

			g_pESP->viewMatrixAddr = tmpViewMatrixAddr;
		}

		g_pD3D->pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0); // Clear the window alpha
		g_pD3D->pD3DDevice->BeginScene(); // Begin the 3D scene
		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		g_pD3D->MenuTheme(); // Custom menu theme
		ImGui::NewFrame();

		// Menu draw
		if (Settings::bShowMenu)
		{	
			ImGui::Begin("mrBump :)", nullptr, ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoResize*/);
			g_pD3D->MenuRender();
			ImGui::End();
		}

		if (bInGame)
		{
			// Cheat feature draw (ESP,...)
			{
				g_pVMM->GetViewMatrix();

				if (Settings::bUnsortedActorESP)
					g_pESP->DrawUnsortedActors();

				g_pESP->DrawItems();

				g_pESP->DrawAirDrop();
				g_pESP->DrawLootbox();

				g_pESP->DrawVehicles();

				// Reset then in DrawPlayers function, we will find new target for aimbot
				g_pAim->ResetTargetValue();
				g_pESP->DrawPlayers();

				//if (g_pAim->targetPos.X != -999 && g_pAim->targetPos.Y != -999)
				//	g_pD3D->DrawLine(g_pD3D->screenW / 2, g_pD3D->screenH / 2, g_pAim->targetPos.X - g_pD3D->screenW / 2, g_pAim->targetPos.Y - g_pD3D->screenH / 2, RED(255));
			}
			
			
			// Current match info
			{
				ImGui::Begin("Game Status", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
				ImGui::TextColored(ImVec4(0.33f, 1.0f, 1.0f, 1.0f), "In Game");
				int nearby = 0;
				nearby = g_pESP->CharacterCount;
				std::string txt{ "Nearby " + std::to_string(nearby) };
				ImGui::Text(txt.c_str());
				ImGui::End();
			}
		}
		else
		// Current match info
		{
			ImGui::Begin("Game Status", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
			ImGui::Text("In Lobby");
			ImGui::End();
		}

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		g_pD3D->pD3DDevice->EndScene(); // End the 3D scene
		g_pD3D->pD3DDevice->Present(NULL, NULL, NULL, NULL); // Display the created frame on the screen

		if ((GetAsyncKeyState(VK_XBUTTON2) & 0x8000) && (g_pAim->targetPos.X > 0 && g_pAim->targetPos.Y > 0))
		{
			long targetX = static_cast<long>(g_pAim->targetPos.X) - g_pD3D->screenW / 2;
			long targetY = static_cast<long>(g_pAim->targetPos.Y) - g_pD3D->screenH / 2;

			//nếu crosshair gần đến điểm aim trên mục tiêu thì hạ tốc độ aimbot
			if ((targetX > -13 && targetX < 13) && (targetY > -13 && targetY < 13)) {
				targetX *= 0.1;
				targetY *= 0.1;
			}

			targetX /= Settings::Aimbot::sensitivity;
			targetY /= Settings::Aimbot::sensitivity;

			mouse_event(MOUSEEVENTF_MOVE, (DWORD)(targetX), (DWORD)(targetY), 0, 0);
		}

		// Resume the read mem loop, let it continue its work :>
		// We put it here because in DrawPlayer function we are using a read memory function so we need to pause the read mem loop longer
		if (g_bDoneReadMem)
			g_bDoneReadMem = false;

		next_frame += std::chrono::milliseconds(Settings::drawLoopDelay);
		std::this_thread::sleep_until(next_frame); // Wait for end of frame
	}

	return 0;
}