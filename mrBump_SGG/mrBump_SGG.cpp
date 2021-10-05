#include "pch.h"

#include "mrBump_SGG.h"

bool g_bDoneReadMem = false;
bool g_bActive = true;

std::array<int, MAX_ACTORS> ActorArray;

std::vector<Character> tmpCharacters;
//int g_tmpCharacterCount = 0;

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

	if (Settings::bDebugESP)
	{
		item.ItemDefineID = g_pMM->read<SDK::ItemDefineID>(item.Address + ITEMDEFINEID);
		item.actorName = currActorName;
	}

	tmpItems.emplace_back(item);
}

void UpdateValue()
{
	while (g_bActive)
	{
		g_pESP->UWorld = g_pMM->read<DWORD>(g_pESP->GWorld);

		// When UWorld == 0, if we read the next value (NetDriver)
		// it will crash the hack (undefined behavior)
		if (g_pESP->UWorld == 0)
			continue;

		DWORD NetDriver{ g_pMM->read<DWORD>(g_pESP->UWorld + NETDRIVER) };
		
		// If player is in a match, NetDriver != 0 
		// It will be glitche when ...
		if (NetDriver != 0)
		{
			bInGame = true;
		}
		else
		{
			bInGame = false;

			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			continue;
		}

		tmpUnsortedActors.clear();
		tmpCharacters.clear();
		tmpVehicles.clear();
		tmpItems.clear();
		tmpAirDrops.clear();
		tmpAirDropDatas.clear();
		tmpLootboxes.clear();

		tmpViewMatrixAddr = g_pMM->read<DWORD>(g_pMM->read<DWORD>(g_pESP->viewWorld) + 32) + 512;
		
		// Get my character address
		DWORD NetConnection{ g_pMM->read<DWORD>(NetDriver + SERVERCONNECTION) };
		DWORD PlayerController{ g_pMM->read<DWORD>(NetConnection + PLAYERCONTROLLER) };
		g_pESP->Pawn = g_pMM->read<DWORD>(PlayerController + ACKNOWLEDGEDPAWN);

		// Get my character head bone world position
		DWORD SkeletalMeshComponent{ g_pMM->read<DWORD>(g_pESP->Pawn + g_pESP->MeshOffset) };
		DWORD bodyAddr{ SkeletalMeshComponent + 0x150 };
		DWORD boneAddr{ g_pMM->read<DWORD>(SkeletalMeshComponent + 1456) + 48 };
		g_pESP->PawnHeadBoneGamePos = g_pVMM->GetBoneGamePosition(bodyAddr, boneAddr + 5 * 48);
		g_pESP->PawnHeadBoneGamePos.Z += 5;

		// Get BulletFireSpeed of Pawn's current weapon
		DWORD WeaponEntity{ g_pMM->read<DWORD>(g_pESP->Pawn + 0x1740) };
		// It will be NULL when Pawn doesn't hold any firearm
		if (WeaponEntity)
		{
			DWORD ShootWeaponEntity{ g_pMM->read<DWORD>(WeaponEntity + 0xCDC) };
			g_pESP->PawnBulletFireSpeed = g_pMM->read<float>(ShootWeaponEntity + 0x3D4);
		}
		else
		{
			g_pESP->PawnBulletFireSpeed = 0.0f;
		}


		// Get ActorList and maxActorCount
		g_pESP->Level = g_pMM->read<DWORD>(g_pESP->UWorld + PERSISTENTLEVEL);
		g_pESP->ActorList = g_pMM->read<DWORD>(g_pESP->Level + g_pESP->ActorListOffset);
		g_pESP->maxActorCount = g_pMM->read<DWORD>(g_pESP->Level + g_pESP->maxActorCountOffset);
		
		// Match info
		/*DWORD AuthorityGameMode = 0x168;
		DWORD GameModeBase = g_pMM->read<DWORD>(g_pESP->UWorld + 0x168);
		DWORD GameStateBase = g_pMM->read<DWORD>(GameModeBase + 0x2ec);*/

		if (g_pESP->maxActorCount > MAX_ACTORS)
			g_pESP->maxActorCount = MAX_ACTORS;

		// Fast fill array with 0
		memset(&ActorArray, 0, sizeof(ActorArray));

		// Read all ActorAddr
		// multiply 4: because size of 1 address is 4 bytes.
		g_pMM->readMemory((PVOID)g_pESP->ActorList, &ActorArray, static_cast<SIZE_T>(g_pESP->maxActorCount)*4);

		/*std::cout << "UWorld " << g_pESP->UWorld << '\n';
		std::cout << "NetDriver " << NetDriver << '\n';
		std::cout << "tmpViewMatrixAddr " << tmpViewMatrixAddr << '\n';
		std::cout << "Pawn " << g_pESP->Pawn << '\n';
		std::cout << "Level " << g_pESP->Level << '\n';
		std::cout << "ActorList " << g_pESP->ActorList << '\n';
		std::cout << "ActorArray " << ActorArray.size() << '\n';
		std::cout << "maxActorCount " << g_pESP->maxActorCount << '\n';*/


		// Loop through ActorArray
		for (int i = 0; i < g_pESP->maxActorCount; ++i)
		{
			DWORD currActorAddr = ActorArray[i];
			if (currActorAddr == NULL)
				continue;

			std::string currActorName{};

			DWORD currActorID{ g_pMM->read<DWORD>(currActorAddr + 0x10) };
			// If current actorID is exist in ActorNameCache
			if (g_pESP->ActorNameCache.find(currActorID) != g_pESP->ActorNameCache.end())
			{
				currActorName = g_pESP->ActorNameCache[currActorID];
			}
			else
			{
				// Not found in ActorNameCache
				// So we will get actorName and add it to ActorNameCache
				currActorName = g_pESP->GetActorName(currActorID);
				g_pESP->ActorNameCache.insert( std::make_pair(currActorID, currActorName) );
			}

			DWORD SceneComponent{ g_pMM->read<DWORD>(currActorAddr + ROOTCOMPONENT) };
			SDK::FVector currActorPos{ g_pMM->read<SDK::FVector>(SceneComponent + ACTORPOSITION) };

			if (g_pESP->IsItem(currActorName) && (Settings::ItemESP::bToggle || Settings::bDebugESP))
			{
				AddToItems(currActorName, currActorAddr, currActorPos);
				
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

	// HandleWindow, HandleKeyInput
	std::thread windowManager(EventManager, &g_bActive);

	// aimbot thread
	std::thread aimBot(AimbotLoop, &g_bActive);

	// Prepare FPS limiter
	using clock = std::chrono::steady_clock;
	auto next_frame = clock::now();

	g_pD3D->MenuTheme(); // Custom menu theme

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
				// Stop draw loop + read mem loop
				g_bActive = false;

				// Stop read mem loop
				g_bDoneReadMem = false;
			}
		}

		// Get data from read mem loop data when it complete reading memory
		if (g_bDoneReadMem)
		{
			g_pESP->UnsortedActors = tmpUnsortedActors;

			g_pESP->Characters = tmpCharacters;
			//g_pESP->CharacterCount = g_tmpCharacterCount;

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
				//ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |/* ImGuiWindowFlags_NoDecoration |*/ /*ImGuiWindowFlags_NoBackground |*/ ImGuiWindowFlags_AlwaysAutoResize);
				/*ImGui::SetNextWindowPos(ImVec2(g_pD3D->gameScreenRct.left, g_pD3D->gameScreenRct.top));
				ImGui::SetNextWindowSize(ImVec2(g_pD3D->screenW, g_pD3D->screenH));
				ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);*/

				g_pVMM->GetViewMatrix();

				g_pD3D->GetImGuiBackgroundDrawList();

				if (Settings::bUnsortedActorESP)
					g_pESP->DrawUnsortedActors();

				g_pESP->DrawItems();

				g_pESP->DrawAirDrop();
				g_pESP->DrawLootbox();

				g_pESP->DrawVehicles();

				g_pESP->DrawPlayers();

				/*ImGui::End();*/
			}
			
			
			// Current match info
			{
				ImGui::Begin("Game Status", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
				ImGui::TextColored(ImVec4(0.33f, 1.0f, 1.0f, 1.0f), "In Game");
				std::string txt{ "Nearby: " + std::to_string(g_pESP->CharacterCount) };
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

		// Resume the read mem loop, let it continue its work :>
		// There are some functions that are reading memory in Draw functions
		// so we need to pause the read mem loop longer.
		if (g_bDoneReadMem)
			g_bDoneReadMem = false;

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		g_pD3D->pD3DDevice->EndScene(); // End the 3D scene
		g_pD3D->pD3DDevice->Present(NULL, NULL, NULL, NULL); // Display the created frame on the screen

		next_frame += std::chrono::milliseconds(Settings::drawLoopDelay);
		std::this_thread::sleep_until(next_frame); // Wait for end of frame
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	g_pD3D->CleanupDeviceD3D();

	return 0;
}