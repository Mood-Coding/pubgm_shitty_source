﻿#include "pch.h"

#include "Offset.h"
#include "D3D.h"
#include "ESP.h"
#include "Aimbot.h"
#include "Settings.h"
#include "mrBump_SGG.h"
#include "MemoryManager.h"
#include "ProcessManager.h"

void DumpActorName()
{
	std::cout << "[Debug] Start dumping" << std::endl;

	std::ofstream myfile;
	myfile.open("ActorName.txt");

	// Increase if missing some actor name
	for (int i = 0; i < 250000; i++)
	{
		myfile << i << "  |  " << g_pESP->GetActorName(i) << "\n";
	}

	myfile.close();

	std::cout << "[Debug] Dump actor name done!" << std::endl;
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

bool g_bDoneReadMem = false;
bool g_bActive = true;

std::vector<Character> tmpCharacters;
std::vector<Vehicle> tmpVehicles;
std::vector<Item> tmpItems;

std::vector<Airdrop> tmpAirDrops;
std::vector<BoxData> tmpLootboxes;
std::vector<BoxData> tmpAirDropDatas;

std::vector<UnsortedActor> tmpUnsortedActors;

//std::vector<Lootbox> newLootboxes;

DWORD tmpViewMatrixAddr = 0;

int g_tmpCharacterCount = 0;

bool bInGame = false;

// Remove Lootbox that is belong to Airdrop
// Then find out what box(airdrop or lootbox) that certain boxData is belong to
// By checking box's position(X, Y) and boxData's position(X, Y)
//void SortBoxData()
//{
//
//	// Move Lootbox that isn't belong to Airdrop to a new Lootboxes vector
//	for (auto& airdrop : tmpAirdrops)
//	{
//		for (auto& lootbox : tmpLootboxes)
//		{
//			if (lootbox.Position.X != airdrop.Position.X && lootbox.Position.Y != airdrop.Position.Y && lootbox.Position.Z != airdrop.Position.Z)
//			{
//				newLootboxes.push_back(lootbox);
//				break;
//			}
//		}
//	}
//
//	// Then find out what box(airdrop or lootbox) that every boxData belong to
//	
//	for (auto &boxData : tmpBoxData)
//	{
//		bool bFound = false;
//
//		for (auto airdrop : tmpAirdrops)
//		{
//			if (boxData.Position.X == airdrop.Position.X && boxData.Position.Y == airdrop.Position.Y)
//			{
//				airdrop.boxData = boxData;
//				bFound = true;
//				break;
//			}
//		}
//
//		// If yes so we dont need to loop through position of lootbox
//		// Because each boxData is only belong to 1 box (airdrop or lootbox)
//		if (bFound)
//			continue;
//
//		for (auto& lootbox : newLootboxes)
//		{
//			if (boxData.Position.X == lootbox.Position.X && boxData.Position.Y == lootbox.Position.Y)
//			{
//				lootbox.boxData = boxData;
//				break;
//			}
//		}
//	}
//}

void UpdateValue()
{
	tmpAirDrops.reserve(20);
	tmpCharacters.reserve(101);
	tmpLootboxes.reserve(101);
	tmpUnsortedActors.reserve(512);
	tmpItems.reserve(200);
	tmpVehicles.reserve(50);

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
			// Reset left over from last match
			if (bInGame)
			{
				g_pESP->ActorCache.clear();

				g_pESP->MyTeamID = 0;
			}

			bInGame = false;

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
		// Because size of actor address is 4 byte: maxActorCount * 4
		for (DWORD pActorAddr = g_pESP->ActorList; pActorAddr <= g_pESP->ActorList + g_pESP->maxActorCount * 4; pActorAddr += 4)
		{
			DWORD currActorAddr = g_pMM->read<DWORD>(pActorAddr);

			if (currActorAddr == NULL)
				continue;

			// Current actor address = my player's address
			if (currActorAddr == g_pESP->Pawn)
			{
				// Didn't have my team id
				if (g_pESP->MyTeamID == 0)
					g_pESP->MyTeamID = g_pMM->read<DWORD>(g_pESP->Pawn + TEAMID);
			}

			ActorCaching cachedActor{};

			bool bIsCached = false;

			DWORD currActorID = 0;
			std::string currActorName{};

			SDK::FVector currActorPos;
			DWORD SceneComponent = 0;

			// If current actor address is exist in actor cache map
			// Return value of ActorCache.count will be 1, else 0
			if (g_pESP->ActorCache.count(currActorAddr))
			{
				bIsCached = true;

				// Get cached data from actor cache map
				cachedActor = g_pESP->ActorCache.at(currActorAddr);

				currActorName = cachedActor.ActorName;
			}
			else
			{
				currActorID = g_pMM->read<DWORD>(currActorAddr + 0x10);
				currActorName = g_pESP->GetActorName(currActorID);
			}

			SceneComponent = g_pMM->read<DWORD>(currActorAddr + ROOTCOMPONENT);
			currActorPos = g_pMM->read<SDK::FVector>(SceneComponent + ACTORPOSITION);

			if (g_pESP->IsPlayer(currActorName) && Settings::PlayerESP::bToggle)
			{
				Character character(currActorAddr, currActorPos);

				// These data below can't be cached because it update every second
				 
				// Read whole STExtraCharacter class
				character.STExtraCharacter = g_pMM->read<SDK::STExtraCharacter>(currActorAddr);

				// This character is in my team so skip
				if (character.STExtraCharacter.TeamID == g_pESP->MyTeamID)
				{
					continue;

				}

				if (Settings::PlayerESP::bName)
					character.PlayerName = g_pESP->GetPlayerName(character.STExtraCharacter.PlayerName);

				bool bDead = g_pMM->read<bool>(currActorAddr + 0x964);
				if (bDead)
					continue;

				if (bIsCached)
				// Get all cached data
				{
					// Nothing o.O
				}
				else
				{	
					// Cache current actor to ActorCache map
					g_pESP->ActorCache[character.Address] = ActorCaching(currActorName, "", L"");
				}

				// emplace_back a local object is faster than push_back a local object :v
				tmpCharacters.emplace_back(character);
				++g_tmpCharacterCount;

				continue;
			}

			if (g_pESP->IsVehicle(currActorName) && Settings::VehicleESP::bToggle)
			{
				Vehicle vehicle(currActorAddr, currActorPos);

				// These data below can't be cached because it update every second
				DWORD VehicleCommonComponent = g_pMM->read<DWORD>(currActorAddr + VEHICLECOMMON);
				vehicle.VehicleCommonComponent = g_pMM->read<SDK::VehicleCommonComponent>(VehicleCommonComponent);

				if (bIsCached)
				{
					vehicle.displayName = cachedActor.DisplayName;
				}
				else
				{
					std::string actorDisplayName = ActorDisplayName[currActorName];
					// Display name unavailable
					if (actorDisplayName != "")
						vehicle.displayName = actorDisplayName;
					else
						vehicle.displayName = currActorName;

					g_pESP->ActorCache[vehicle.Address] = ActorCaching(currActorName, vehicle.displayName, L"");
				}

				// emplace_back a local object is faster than push_back a local object :v
				tmpVehicles.emplace_back(vehicle);

				continue;
			}

			if (g_pESP->IsAirdrop(currActorName))
			{
				Airdrop airdrop(currActorAddr, currActorPos);

				tmpAirDrops.emplace_back(airdrop);

				continue;
			}

			if (g_pESP->IsAirDropData(currActorName))
			{	
				BoxData airDropData{ currActorAddr, currActorPos };

				g_pESP->GetBoxItems(&airDropData);

				tmpAirDropDatas.emplace_back(airDropData);

				continue;
			}

			if (g_pESP->IsLootbox(currActorName))
			{
				BoxData lootboxData(currActorAddr, currActorPos);

				g_pESP->GetBoxItems(&lootboxData);

				tmpLootboxes.emplace_back(lootboxData);

				continue;
			}


			if (g_pESP->IsItem(currActorName, cachedActor.bIsItem, bIsCached) && Settings::ItemESP::bToggle)
			{
				Item item(currActorAddr, currActorPos);

				if (bIsCached)
				{
					item.displayName = cachedActor.DisplayName;
				}
				else
				{
					std::string actorDisplayName = ActorDisplayName[currActorName];
					if (actorDisplayName != "")
						item.displayName = actorDisplayName;
					else
						item.displayName = currActorName;

					g_pESP->ActorCache[item.Address] = ActorCaching(currActorName, item.displayName, true);
				}

				tmpItems.emplace_back(item);
				
				continue;
			}

			UnsortedActor unsortedActor(currActorName, currActorPos);
			tmpUnsortedActors.emplace_back(unsortedActor);
		}

		//TODO sort box data to airdrop and lootbox
		//SortBoxData();

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
	std::wstring emulator{};

	// Find Smartgaga HWND
	HWND targetHWND = FindWindow(L"TitanRenderWindowClass", NULL);
	targetHWND = FindWindowEx(targetHWND, 0, L"TitanOpenglWindowClass", NULL);

	if (targetHWND)
	{
		emulator = L"AndroidProcess.exe";
	}
	else
	{
		// Find Gameloop HWND
		targetHWND = FindWindow(L"TXGuiFoundation", L"Gameloop");
		targetHWND = FindWindowEx(targetHWND, NULL, L"AEngineRenderWindowClass", L"AEngineRenderWindow");

		emulator = L"aow_exe.exe";
	}

    if (!g_pD3D->SetupHWND(targetHWND))
	{
		system("pause");
        return 0;
    }

	if (!g_pPM->Init(emulator))
	{
		system("pause");
		return 0;
	}

	if (!g_pMM->Init(g_pPM->hProcess, g_pPM->processPID))
	{
		system("pause");
		return 0;
	}

	if (!g_pESP->Init(emulator))
	{
		system("pause");
		return 0;
	}

	if (!g_pAim->Init())
	{
		system("pause");
		return 0;
	}
	
	// Start read memory loop
	std::thread readMem(UpdateValue);

	// FPS limiter
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

		// When we minimized emulator window, its current hwnd will be invalid
		// So we need to get the new emulator hwnd
		if (emulator == L"aow_exe.exe")
		{
			targetHWND = FindWindow(L"TXGuiFoundation", L"Gameloop");
			targetHWND = FindWindowEx(targetHWND, NULL, L"AEngineRenderWindowClass", L"AEngineRenderWindow");
		}
		else
		{
			targetHWND = FindWindow(L"TitanRenderWindowClass", NULL);
			targetHWND = FindWindowEx(targetHWND, 0, L"TitanOpenglWindowClass", NULL);
		}
		g_pD3D->gameHWND = targetHWND;

		GetWindowRect(g_pD3D->gameHWND, &g_pD3D->gameScreenRct);

		//SetWindowPos(g_pD3D->overlayHWND, 0, g_pD3D->gameScreenRct.left, g_pD3D->gameScreenRct.top, g_pD3D->screenW, g_pD3D->screenH, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSIZE);
		// Move overlay window when we move emulator window
		MoveWindow(g_pD3D->overlayHWND, g_pD3D->gameScreenRct.left, g_pD3D->gameScreenRct.top, g_pD3D->screenW, g_pD3D->screenH, true);

		UpdateWindow(g_pD3D->overlayHWND);

		// Auto close hack when emulator process is closed
		if (targetHWND == NULL)
			::PostQuitMessage(0);
		
		if (GetAsyncKeyState(VK_HOME) & 1)
		{
			Settings::bShowMenu = !Settings::bShowMenu;
			ChangeClickability(Settings::bShowMenu, g_pD3D->overlayHWND);
		}
		
		if (GetAsyncKeyState(VK_END) & 1)
		{
			::PostQuitMessage(0);
		}

		// Get read mem loop's data when it complete reading memory
		if (g_bDoneReadMem)
		{
			g_pESP->Characters = tmpCharacters;
			g_pESP->Items = tmpItems;
			g_pESP->Vehicles = tmpVehicles;
			g_pESP->UnsortedActors = tmpUnsortedActors;

			g_pESP->Lootboxes = tmpLootboxes;
			g_pESP->AirDropDatas = tmpAirDropDatas;
			

			g_pAim->Characters = tmpCharacters;

			g_pESP->CharacterCount = g_tmpCharacterCount;

			g_pESP->viewMatrixAddr = tmpViewMatrixAddr;

			//g_pESP->BoxData = tmpBoxData;
			// Resume the read mem loop, let it continue its work :>
			
		}

		g_pD3D->pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0); // Clear the window alpha
		g_pD3D->pD3DDevice->BeginScene(); // Begin the 3D scene

		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		g_pD3D->MenuTheme();
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

				g_pESP->DrawPlayers();

				if (g_bDoneReadMem)
					g_bDoneReadMem = false;
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

		next_frame += std::chrono::milliseconds(Settings::drawLoopDelay);
		std::this_thread::sleep_until(next_frame); // Wait for end of frame

		g_pAim->ResetTargetValue();
	}

	return 0;
}