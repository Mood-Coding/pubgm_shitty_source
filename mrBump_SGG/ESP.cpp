﻿#include "pch.h"
#include "ESP.h"
#include "Aimbot.h"

ESP* g_pESP = new ESP();

bool ESP::Init(std::wstring emulator)
{
	std::cout << "[ESP]\n";

	viewWorld = g_pVMM->GetViewWorld(emulator);

	if (viewWorld)
	{
		std::cout << "Viewworld base found at: 0x" <<  std::hex << viewWorld << std::hex << "\n";
	}
	else
	{
		std::cout << "Game not found ?" << std::endl;
		return 0;
	}

	GWorld = viewWorld - GWORLD;
	GNames = viewWorld - GNAMES;
	Names = g_pMM->read<DWORD>(GNames);

	std::cout << "GWorld : 0x" << std::hex << GWorld <<  "\n";
	std::cout << "GNames : 0x" << std::hex << GNames << "\n";
	std::cout << "Names : 0x" << std::hex << Names << "\n";

	Airdrops.reserve(20);
	Characters.reserve(101);
	Lootboxes.reserve(101);
	UnsortedActors.reserve(512);
	Items.reserve(200);
	Vehicles.reserve(50);

	return 1;
}

void ESP::DrawItems()
{
	for (int i = 0; i < Items.size(); ++i)
	{	
		g_pVMM->WorldToScreen(Items[i].Position, Items[i].PositionOnSc, Items[i].distance);

		if (Items[i].PositionOnSc.X <= 0 || Items[i].PositionOnSc.Y <= 0)
			continue;

		if (Items[i].distance > Settings::ItemESP::drawDistance /*|| Items[i].distance < 0*/)
			continue;

		// Optimize
		unsigned int color = 0;
		int colorFilterID = ActorColorFilterID[Items[i].displayName];
		if (colorFilterID < 1)
			color = DARKRED(255);
		else
			color = ColorFilter[colorFilterID];

		int xOffset = 0;

		if (Settings::ItemESP::bName)
		{
			std::string str{ Items[i].displayName };

			g_pD3D->DrawString(Items[i].PositionOnSc.X + xOffset, Items[i].PositionOnSc.Y, color, str, Settings::bToggleShadowText);
			xOffset += ImGui::CalcTextSize(str.c_str()).x + 5;
		}

		if (Settings::ItemESP::bDistance)
		{
			std::string str{ std::to_string(Items[i].distance) + "m" };

			g_pD3D->DrawString(Items[i].PositionOnSc.X + xOffset, Items[i].PositionOnSc.Y, color, str, Settings::bToggleShadowText);
			xOffset += ImGui::CalcTextSize(str.c_str()).x + 5;
		}
	}
}

void ESP::DrawVehicles()
{
	for (int i = 0; i < Vehicles.size(); ++i)
	{
		g_pVMM->WorldToScreen(Vehicles[i].Position, Vehicles[i].PositionOnSc, Vehicles[i].distance);

		if (Vehicles[i].PositionOnSc.X <= 0 || Vehicles[i].PositionOnSc.Y <= 0)
			continue;

		if (Vehicles[i].distance > Settings::VehicleESP::drawDistance)
			continue;
		
		// Footer 1st text
		{
			int xOffset = 0;

			if (Settings::VehicleESP::bName)
			{
				std::string str = Vehicles[i].displayName;
				g_pD3D->DrawString(Vehicles[i].PositionOnSc.X, Vehicles[i].PositionOnSc.Y, LAWNGREEN(255), str, Settings::bToggleShadowText);
				xOffset += ImGui::CalcTextSize(str.c_str()).x + 5;
			}

			if (Settings::VehicleESP::bDistance)
			{
				std::string str = std::to_string(Vehicles[i].distance) + "m";
				g_pD3D->DrawString(Vehicles[i].PositionOnSc.X + xOffset, Vehicles[i].PositionOnSc.Y, LAWNGREEN(255), str, Settings::bToggleShadowText);
			}
		}

		// Footer 2nd text
		{
			int xOffset = 0;

			if (Settings::VehicleESP::bHp)
			{
				int hpPercent = (int)(Vehicles[i].VehicleCommonComponent.HP / Vehicles[i].VehicleCommonComponent.HPMax * 100);

				if (hpPercent < 0)
					continue;

				std::string str{ "HP:" + std::to_string(hpPercent) + '%' };
				g_pD3D->DrawString(Vehicles[i].PositionOnSc.X, Vehicles[i].PositionOnSc.Y + 15, ImColor::HSV( ((6*(float)(hpPercent))/5)/360 , 1, 1), str, Settings::bToggleShadowText);

				xOffset += ImGui::CalcTextSize(str.c_str()).x + 5;
			}

			if (Settings::VehicleESP::bFuel)
			{
				int fuelPercent = (int)(Vehicles[i].VehicleCommonComponent.Fuel / Vehicles[i].VehicleCommonComponent.FuelMax * 100);

				std::string str = "Fuel:" + std::to_string(fuelPercent) + '%';
				g_pD3D->DrawString(Vehicles[i].PositionOnSc.X + xOffset, Vehicles[i].PositionOnSc.Y + 15, ImColor::HSV(((6 * (float)(fuelPercent)) / 5) / 360, 1, 1), str, Settings::bToggleShadowText);
			}
		}
	}
}

void ESP::DrawPlayers()
{
	for (int i = 0; i < Characters.size(); ++i)
	{
		g_pVMM->WorldToScreenPlayer(Characters[i].Position, Characters[i].PositionOnSc, Characters[i].distance);

		if ( (Characters[i].PositionOnSc.X == 0 && Characters[i].PositionOnSc.Y == 0) /*|| Characters[i].distance >= 1000*/)
			continue;

		// Player name + Bot check
		{
			long txtBotCheckOffset = 0;

			if (Settings::PlayerESP::bName)
			{
				RECT txtRct{};
				g_pD3D->pPlayerNameFont->DrawText(NULL, Characters[i].PlayerName.c_str(), Characters[i].PlayerName.length(), &txtRct, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));

				// We use D3D draw string API because ImGui doesn't support UNICODE string
				g_pD3D->DrawString(Characters[i].PositionOnSc.X - floor((txtRct.right - txtRct.left) / 2), Characters[i].PositionOnSc.Y - 32, WHITE(255), Characters[i].PlayerName, Settings::bToggleShadowText);

				txtBotCheckOffset += floor((txtRct.right - txtRct.left) / 2) + 10;
			}

			if (Characters[i].STExtraCharacter.bIsAI)
			{
				g_pD3D->DrawString(Characters[i].PositionOnSc.X - txtBotCheckOffset, Characters[i].PositionOnSc.Y - 32, CYAN(255), "B", false);
			}
		}
		
		// Distance + HP
		{
			std::string topTxt{};

			if (Settings::PlayerESP::bDistance)
				topTxt += std::to_string(Characters[i].distance) + "m";

			if (Settings::PlayerESP::bHp)
			{
				if (!topTxt.empty())
					topTxt += " ";

				topTxt += std::to_string((int)(Characters[i].STExtraCharacter.Health)) + "HP";
			}

			if (!topTxt.empty())
			{
				ImVec2 size = ImGui::CalcTextSize(topTxt.c_str());
				g_pD3D->DrawString(Characters[i].PositionOnSc.X - floor(size.x / 2), Characters[i].PositionOnSc.Y - 18, RED(255), topTxt, Settings::bToggleShadowText);
			}
		}

		if (Settings::PlayerESP::BoneESP::bToggle)
		{
			// Because read mem loop has high delay than draw loop
			// Puting GetPlayerBonePos here so Player's bone ESP will be more smooth than putting this in read mem loop
			GetPlayerBonePos(&Characters[i]);
			DrawPlayerBone(&Characters[i]);

			g_pAim->FindBestTarget(&Characters[i]);
		}

		// Knocked check (hp <= 0)
		if (Characters[i].STExtraCharacter.Health <= 0)
		{
			g_pD3D->DrawString(Characters[i].PositionOnSc.X - floor(53 / 2), Characters[i].PositionOnSc.Y + Characters[i].PositionOnSc.Z * 2, RED(255), "Knocked", Settings::bToggleShadowText);
		}

		// Line
		if (Settings::PlayerESP::LineESP::bToggle)
		{
			g_pD3D->DrawLine(Characters[i].PositionOnSc.X, Characters[i].PositionOnSc.Y - 20, g_pD3D->screenW / 2, 0, WHITE(255));
		}
	}
}

void ESP::DrawUnsortedActors()
{
	for (int i = 0; i < UnsortedActors.size(); ++i)
	{
		g_pVMM->WorldToScreen(UnsortedActors[i].Position, UnsortedActors[i].PositionOnSc, UnsortedActors[i].distance);

		g_pD3D->DrawString(UnsortedActors[i].PositionOnSc.X, UnsortedActors[i].PositionOnSc.Y, PINK(255), UnsortedActors[i].ActorName, false);
	}
}

void ESP::DrawAirDrop()
{
	for (int i = 0; i < AirDropDatas.size(); ++i)
	{
		g_pVMM->WorldToScreen(AirDropDatas[i].Position, AirDropDatas[i].PositionOnSc, AirDropDatas[i].distance);
		g_pD3D->DrawString(AirDropDatas[i].PositionOnSc.X + 18, AirDropDatas[i].PositionOnSc.Y, RED(255), "Data", false);
	}

	for (int i = 0; i < Airdrops.size(); ++i)
	{
		g_pVMM->WorldToScreen(Airdrops[i].Position, Airdrops[i].PositionOnSc, Airdrops[i].distance);
		g_pD3D->DrawString(Airdrops[i].PositionOnSc.X, Airdrops[i].PositionOnSc.Y, RED(255), "AirDrop", true);
		//g_pD3D->DrawString(Airdrops[i].PositionOnSc.X, Airdrops[i].PositionOnSc.Y + 18, RED(255), std::to_string(Airdrops[i].boxData.itemCount), false);
		/*g_pD3D->DrawString(Airdrops[i].PositionOnSc.X, Airdrops[i].PositionOnSc.Y + 18 + 18, RED(255), std::to_string(Airdrops[i].count), false); */

		/*int xOffset = 0;*/

		/*for (auto itr : Airdrops[i].Items)
		{
			g_pD3D->DrawString(Airdrops[i].PositionOnSc.X, Airdrops[i].PositionOnSc.Y + 18 + 18 + 18 + xOffset, RED(255), itr.first + ':' + itr.second, false);
			xOffset += 18;
		}*/
	}
}

void ESP::DrawLootbox()
{
	for (int i = 0; i < Lootboxes.size(); ++i)
	{
		g_pVMM->WorldToScreen(Lootboxes[i].Position, Lootboxes[i].PositionOnSc, Lootboxes[i].distance);

		g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y, RED(255), "Lootbox", true);

		int xOffset = 0;
		for (auto itr : Lootboxes[i].items)
		{
			g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18 + 18 + 18 + xOffset, RED(255), itr, false);
			xOffset += 18;
		}

		if (Settings::bDebugESP)
		{
			g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18, RED(255), std::to_string(Lootboxes[i].address), false);

			g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18 + 18, RED(255), std::to_string(Lootboxes[i].itemCount), false);
		}
	}
}

std::string ESP::GetActorName(DWORD actorID)
{
	DWORD FNamePtr = g_pMM->read<DWORD>(Names + (actorID / 0x4000) * 4);	
	DWORD FName = g_pMM->read<DWORD>(FNamePtr + 4 * (actorID % 0x4000));
	FName += 8;

	char name[70] {};

	// minus 1 : or string will end with ||||||||||||||||||||||||||||||||||||||||||||
	g_pMM->readMemory((PVOID)FName, &name, sizeof(name)-1);

	std::string result = std::string(name);
	return result;
}

std::wstring ESP::GetPlayerName(DWORD nameAddr)
{
	TCHAR p[16] {};
	g_pMM->readMemory((PVOID)nameAddr, &p, sizeof(p) - 1);

	std::wstring result1 = p;
	return result1;
}

void ESP::DrawHeadBone(SDK::FVector2D headScreenPosition, int playerDistance)
{
	float size = 50.f;
	float scale = playerDistance * 0.5;
	if (scale == 0) {
		scale = 1;
	}
	size /= scale;
	g_pD3D->DrawCircle(headScreenPosition.X, headScreenPosition.Y, size, RED(255));
}

void ESP::DrawPlayerBone(Character* character)
{
	if (character->BONE_HEAD.X == 0 && character->BONE_HEAD.Y == 0)
		return;

	if (character->BONE_CHEST.X == 0 && character->BONE_CHEST.Y == 0)
		return;

	if (character->BONE_PELVIS.X == 0 && character->BONE_PELVIS.Y == 0)
		return;

	DrawHeadBone(character->BONE_HEAD, character->distance);

	g_pD3D->DrawLine(character->BONE_CHEST.X, character->BONE_CHEST.Y, character->BONE_PELVIS.X, character->BONE_PELVIS.Y, WHITE(255));

	g_pD3D->DrawLine(character->BONE_CHEST.X, character->BONE_CHEST.Y, character->BONE_L_ELBOW.X, character->BONE_L_ELBOW.Y, WHITE(255));
	g_pD3D->DrawLine(character->BONE_CHEST.X, character->BONE_CHEST.Y, character->BONE_R_ELBOW.X, character->BONE_R_ELBOW.Y, WHITE(255));

	g_pD3D->DrawLine(character->BONE_L_ELBOW.X, character->BONE_L_ELBOW.Y, character->BONE_L_WRIST.X, character->BONE_L_WRIST.Y, WHITE(255));
	g_pD3D->DrawLine(character->BONE_R_ELBOW.X, character->BONE_R_ELBOW.Y, character->BONE_R_WRIST.X, character->BONE_R_WRIST.Y, WHITE(255));

	g_pD3D->DrawLine(character->BONE_PELVIS.X, character->BONE_PELVIS.Y, character->BONE_L_KNEE.X, character->BONE_L_KNEE.Y, WHITE(255));
	g_pD3D->DrawLine(character->BONE_PELVIS.X, character->BONE_PELVIS.Y, character->BONE_R_KNEE.X, character->BONE_R_KNEE.Y, WHITE(255));

	g_pD3D->DrawLine(character->BONE_L_KNEE.X, character->BONE_L_KNEE.Y, character->BONE_L_FOOT.X, character->BONE_L_FOOT.Y, WHITE(255));
	g_pD3D->DrawLine(character->BONE_R_KNEE.X, character->BONE_R_KNEE.Y, character->BONE_R_FOOT.X, character->BONE_R_FOOT.Y, WHITE(255));
}

void ESP::GetPlayerBonePos(Character* character)
{
	DWORD SkeletalMeshComponent = g_pMM->read<DWORD>(character->Address + g_pESP->MeshOffset);
	DWORD bodyAddr = SkeletalMeshComponent + 0x150;
	DWORD boneAddr = g_pMM->read<DWORD>(SkeletalMeshComponent + 1456) + 48;

	//iterate over BONES enum
	for (int curBone = 0; curBone < 11; ++curBone) {
		Vector3f curBoneWorldPos = g_pVMM->GetBoneWorldPosition(bodyAddr, boneAddr + Bones[curBone] * 48);
		switch (Bones[curBone])
		{
		case BONE_HEAD:
		{
			curBoneWorldPos.z += 8;
			g_pVMM->WorldToScreenBone(curBoneWorldPos, character->BONE_HEAD);
			break;
		}
		case BONE_CHEST:
		{
			g_pVMM->WorldToScreenBone(curBoneWorldPos, character->BONE_CHEST);
			break;
		}
		case BONE_PELVIS:
		{
			g_pVMM->WorldToScreenBone(curBoneWorldPos, character->BONE_PELVIS);
			break;
		}
		case BONE_L_ELBOW:
		{
			g_pVMM->WorldToScreenBone(curBoneWorldPos, character->BONE_L_ELBOW);
			break;
		}
		case BONE_L_WRIST:
		{
			g_pVMM->WorldToScreenBone(curBoneWorldPos, character->BONE_L_WRIST);
			break;
		}
		case BONE_R_ELBOW:
		{
			g_pVMM->WorldToScreenBone(curBoneWorldPos, character->BONE_R_ELBOW);
			break;
		}
		case BONE_R_WRIST:
		{
			g_pVMM->WorldToScreenBone(curBoneWorldPos, character->BONE_R_WRIST);
			break;
		}
		case BONE_L_KNEE:
		{
			g_pVMM->WorldToScreenBone(curBoneWorldPos, character->BONE_L_KNEE);
			break;
		}
		case BONE_L_FOOT:
		{
			g_pVMM->WorldToScreenBone(curBoneWorldPos, character->BONE_L_FOOT);
			break;
		}
		case BONE_R_KNEE:
		{
			g_pVMM->WorldToScreenBone(curBoneWorldPos, character->BONE_R_KNEE);
			break;
		}
		case BONE_R_FOOT: {
			g_pVMM->WorldToScreenBone(curBoneWorldPos, character->BONE_R_FOOT);
			break;
		}
		}
	}
}

int ESP::Bones[11]
{
	BONE_HEAD,
	BONE_CHEST,
	BONE_PELVIS,
	BONE_L_ELBOW,
	BONE_L_WRIST,
	BONE_R_ELBOW,
	BONE_R_WRIST,
	BONE_L_KNEE,
	BONE_L_FOOT,
	BONE_R_KNEE,
	BONE_R_FOOT
};

bool ESP::IsVehicle(const std::string& actorName)
{
	if (actorName.find('/') != std::string::npos)
		return false;

	if (actorName.find("VH_") != std::string::npos
		||actorName.find("Mirado") != std::string::npos
		||actorName.find("water_Plane") != std::string::npos
		||actorName.find("AquaRail") != std::string::npos
		||actorName.find("Rony") != std::string::npos)
	{
		return true;
	}

	return false;
}

bool ESP::IsPlayer(const std::string& actorName)
{
	if (actorName.find("PlayerCharacter") != std::string::npos)
		return true;

	if (actorName.find("FakePlayer") != std::string::npos)
		return true;

	if (actorName.find("PlayerPawn") != std::string::npos)
		return true;

	return false;
}

bool ESP::IsItem(const std::string& actorName, bool bIsItem, bool bIsCached)
{
	if (bIsCached)
		if (bIsItem)
			return true;
		else
			return false;

	// Although this actor name containts: "PickUp" and "Wrapper" but it's airdrop/ lootbox actor.

	if (actorName.find("Wrapper") == std::string::npos
		&& actorName.find("PickUp") == std::string::npos
		&& actorName.find("Pickup") == std::string::npos)
		return false;

	return true;
}

bool ESP::IsAirDropData(const std::string& actorName)
{
	if (actorName == "AirDropListWrapperActor")
		return true;

	return false;
}

bool ESP::IsAirdrop(const std::string& actorName)
{
	if (actorName == "BP_AirDropBox_C" || actorName == "BP_AirDropBox_C_Recycled")
		return true;

	return false;
}

// Because sometime Airdrop will have 2 actors: PlayerDeadInventoryBox_C and BP_AirDropBox_C
// So we dont use PlayerDeadInventoryBox_C to find Lootbox
// Instead, we will use PickUpListWrapperActor
bool ESP::IsLootbox(const std::string& actorName)
{
	// F :) PlayerDeadInventoryBox_C
	if (actorName == "PickUpListWrapperActor" || actorName == "PickUpListWrapperActor_Recycled")
		return true;

	return false;
}

std::string BoxItemIDToDisplayName()
{
	std::string result{};



	return result;
}

void ESP::GetBoxItems(BoxData* boxData)
{
	// Class: PickUpListWrapperActor.PickUpWrapperActor.UAENetActor.LuaActor.Actor.Object
	DWORD PickUpDataList{ g_pMM->read<DWORD>(boxData->address + PICKUPDATALIST) };
	int itemCount{ g_pMM->read<int>(boxData->address + PICKUPDATALIST + 0x4) }; // It's right before PickUpDataList

	// Copy pasta :D dont know why
	if (itemCount > 32)
		itemCount = 32;

	for (DWORD itemAddr = PickUpDataList; itemAddr <= PickUpDataList + itemCount * 0x30; itemAddr += 0x30)
	{
		//Class: PickUpItemData
		//Class: ItemDefineID //ItemDefineID ID;//[Offset: 0x0, Size: 24]
			//int Type;//[Offset: 0x0, Size: 4]
			int TypeSpecificID{ g_pMM->read<int>(itemAddr + 0x4) }; ///[Offset: 0x4, Size: 4]
			//bool bValidItem;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x8, Size: 1]
			//bool bValidInstance;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x9, Size: 1]
			//uint64 InstanceID;//[Offset: 0x10, Size: 8]
		//int Count{ g_pMM->read<int>(itemAddr + 0x18) }; //[Offset: 0x18, Size: 4]
		//BattleItemAdditionalData[] AdditionalDataList;//[Offset: 0x1c, Size: 12]
		//int InstanceID;//[Offset: 0x28, Size: 4]

		if (TypeSpecificID > 0)
		{
			int Count{ g_pMM->read<int>(itemAddr + 0x18) }; //[Offset: 0x18, Size: 4]
			boxData->items.push_back(std::to_string(TypeSpecificID) + " " + std::to_string(Count));
			++boxData->itemCount;
		}
	}
}