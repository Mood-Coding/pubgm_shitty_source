#include "pch.h"

#include "ESP.h"
#include "Aimbot.h"

ESP* g_pESP = new ESP();

bool ESP::Init()
{
	std::cout << "[ESP]\n";

	viewWorld = g_pVMM->GetViewWorld(g_pPM->emuProcessName);
	if (viewWorld)
	{
		std::cout << "Viewworld base found at: 0x" << std::hex << viewWorld << '\n';
	}
	else
	{
		std::cout << "<GetViewWorld> Game not found!\n";
		return 0;
	}

	GWorld = viewWorld - GWORLD;
	GNames = viewWorld - GNAMES;
	Names = g_pMM->read<DWORD>(GNames);

	std::cout << "GWorld: 0x" << std::hex << GWorld <<  "\n";
	std::cout << "GNames: 0x" << std::hex << GNames << "\n";
	std::cout << "Names: 0x" << std::hex << Names << "\n";

	Airdrops.reserve(20);
	AirDropDatas.reserve(20);
	Lootboxes.reserve(101);
	UnsortedActors.reserve(512);
	Characters.reserve(101);
	Items.reserve(200);
	Vehicles.reserve(50);

	ActorNameCache.reserve(512);

	return true;
}

void ESP::DrawItems()
{
	if (Settings::bDebugESP)
	{
		for (int i = 0; i < Items.size(); ++i)
		{
			if (!g_pVMM->WorldToScreen(Items[i].Position, Items[i].PositionOnSc, Items[i].distance))
				continue;

			if (Items[i].distance > Settings::ItemESP::drawDistance)
				continue;

			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y, WHITE(255), Utils::DecToHex<DWORD>(Items[i].Address).c_str(), Settings::bToggleShadowText);
			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y + 18, WHITE(255), Items[i].actorName, Settings::bToggleShadowText);
			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y + 18 + 18, WHITE(255), Items[i].displayName, Settings::bToggleShadowText);
			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y + 18 + 18 + 18, WHITE(255), std::to_string(Items[i].ItemDefineID.TypeSpecificID).c_str(), Settings::bToggleShadowText);
			std::string txt{ std::to_string(Items[i].PositionOnSc.X) + ' ' + std::to_string(Items[i].PositionOnSc.Y) };
			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y + 18 + 18 + 18 + 18, WHITE(255), txt.c_str(), Settings::bToggleShadowText);
		}

		return;
	}

	for (int i = 0; i < Items.size(); ++i)
	{	
		if (!g_pVMM->WorldToScreen(Items[i].Position, Items[i].PositionOnSc, Items[i].distance))
			continue;

		if (Items[i].distance > Settings::ItemESP::drawDistance)
			continue;

		unsigned int itemDisplayColor{ ColorFilter[ActorColorFilterID[Items[i].displayName]] };
		if (itemDisplayColor == 0)
			itemDisplayColor = DARKRED(255);

		std::string str;

		if (Settings::ItemESP::bName)
			str += Items[i].displayName + " ";

		if (Settings::ItemESP::bDistance)
			str += std::to_string(Items[i].distance) + "m";

		if (str != "")
			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y, itemDisplayColor, str, Settings::bToggleShadowText);
	}
}

void ESP::DrawVehicles()
{
	if (Settings::bDebugESP)
	{
		for (int i = 0; i < Vehicles.size(); ++i)
		{
			if (!g_pVMM->WorldToScreen(Vehicles[i].Position, Vehicles[i].PositionOnSc, Vehicles[i].distance))
				continue;

			if (Vehicles[i].distance > Settings::VehicleESP::drawDistance)
				continue;

			g_pD3D->DrawString(Vehicles[i].PositionOnSc.X, Vehicles[i].PositionOnSc.Y, WHITE(255), Utils::DecToHex<DWORD>(Vehicles[i].Address).c_str(), Settings::bToggleShadowText);
		}

		return;
	}

	for (int i = 0; i < Vehicles.size(); ++i)
	{
		if (!g_pVMM->WorldToScreen(Vehicles[i].Position, Vehicles[i].PositionOnSc, Vehicles[i].distance))
			continue;

		if (Vehicles[i].displayName == "AirDrop Plane")
		{
			g_pD3D->DrawString(Vehicles[i].PositionOnSc.X, Vehicles[i].PositionOnSc.Y, LAWNGREEN(255), "AirDrop Plane", Settings::bToggleShadowText);
			continue;
		}

		if (Vehicles[i].distance > Settings::VehicleESP::drawDistance)
			continue;

		// Footer 1st text
		{
			float xOffset = 0;

			if (Settings::VehicleESP::bName)
			{
				std::string str = Vehicles[i].displayName;
				g_pD3D->DrawString(Vehicles[i].PositionOnSc.X, Vehicles[i].PositionOnSc.Y, LAWNGREEN(255), str, Settings::bToggleShadowText);
				xOffset += ImGui::CalcTextSize(str.c_str()).x + 5.0f;
			}

			if (Settings::VehicleESP::bDistance)
			{
				std::string str = std::to_string(Vehicles[i].distance) + "m";
				g_pD3D->DrawString(Vehicles[i].PositionOnSc.X + xOffset, Vehicles[i].PositionOnSc.Y, LAWNGREEN(255), str, Settings::bToggleShadowText);
			}
		}

		// Footer 2nd text
		{
			float xOffset = 0;

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
	CharacterCount = 0;

	// Debug ESP
	if (Settings::bDebugESP)
	{
		for (int i = 0; i < Characters.size(); ++i)
		{
			++CharacterCount;

			if (!g_pVMM->WorldToScreenPlayer(Characters[i].Position, Characters[i].PositionOnSc, Characters[i].distance))
				continue;

			g_pD3D->DrawString(Characters[i].PositionOnSc.X, Characters[i].PositionOnSc.Y, WHITE(255), Utils::DecToHex<DWORD>(Characters[i].Address).c_str(), Settings::bToggleShadowText);
			g_pD3D->DrawString(Characters[i].PositionOnSc.X, Characters[i].PositionOnSc.Y + 18, WHITE(255), std::to_string(Characters[i].TeamIDIndex).c_str(), Settings::bToggleShadowText);
		}

		return;
	}

	int yOffset{ 0 };

	for (int i = 0; i < Characters.size(); ++i)
	{
		++CharacterCount;

		if (!g_pVMM->WorldToScreenPlayer(Characters[i].Position, Characters[i].PositionOnSc, Characters[i].distance))
			continue;

		unsigned int teamIDColor;

		if (Characters[i].STExtraCharacter.bIsAI)
			teamIDColor = CYAN(255);
		else
			teamIDColor = Characters[i].TeamColor;

		yOffset = 0;

		// Player name
		if (Settings::PlayerESP::bName)
		{
			RECT txtRct{};
			// Calculate PlayerName text rect
			g_pD3D->pPlayerNameFont->DrawTextW(NULL, Characters[i].PlayerName.c_str(), (INT)Characters[i].PlayerName.length(), &txtRct, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));

			// Bot has cyan name, human is white
			// We use D3D draw string API because ImGui doesn't support UNICODE string :< 
			if (Characters[i].STExtraCharacter.bIsAI)
				g_pD3D->DrawString(Characters[i].PositionOnSc.X - (float)floor((txtRct.right - txtRct.left) / 2), Characters[i].PositionOnSc.Y + Characters[i].PositionOnSc.Z, D3DCOLOR_ARGB(255, 0, 255, 255), Characters[i].PlayerName, Settings::bToggleShadowText);
			else
				g_pD3D->DrawString(Characters[i].PositionOnSc.X - (float)floor((txtRct.right - txtRct.left) / 2), Characters[i].PositionOnSc.Y + Characters[i].PositionOnSc.Z, WHITE(255), Characters[i].PlayerName, Settings::bToggleShadowText);

			// Next text line will be under Player name + Bot check
			yOffset += 18;
		}
		
		// Distance + HP
		{
			std::string str{};

			// Distance
			if (Settings::PlayerESP::bDistance)
				str += std::to_string(Characters[i].distance) + "m ";

			// Hp
			if (Settings::PlayerESP::bHp)
				str += std::to_string((int)(Characters[i].STExtraCharacter.Health)) + "HP";

			// Draw distance + hp
			if (str != " ")
			{
				// For centered text at the middle of player
				ImVec2 size{ ImGui::CalcTextSize(str.c_str()) };
				g_pD3D->DrawString(Characters[i].PositionOnSc.X - floor(size.x / 2), Characters[i].PositionOnSc.Y + Characters[i].PositionOnSc.Z + yOffset, WHITE(255), str, Settings::bToggleShadowText);

				yOffset += 18;
			}
		}

		// Knocked check (hp <= 0)
		if (Characters[i].STExtraCharacter.Health <= 0)
		{
			g_pD3D->DrawString(Characters[i].PositionOnSc.X - (float)floor(53 / 2), Characters[i].PositionOnSc.Y + Characters[i].PositionOnSc.Z + yOffset, RED(255), "Knocked", Settings::bToggleShadowText);

			yOffset += 18;
		}

		// Line ESP
		if (Settings::PlayerESP::LineESP::bToggle)
			g_pD3D->DrawLine(Characters[i].PositionOnSc.X, Characters[i].PositionOnSc.Y - 29, (float)g_pD3D->screenW / 2, 0, Characters[i].TeamColor);

		// Bone ESP
		if (Settings::PlayerESP::BoneESP::bToggle || Settings::Aimbot::bToggle)
		{
			// Because read mem loop has higher delay than draw loop, BoneESP will be smoother at here
			if (GetPlayerBonePos(&Characters[i]))
			{
				if (Settings::Aimbot::bToggle)
					g_pAim->FindBestTarget(&Characters[i]);

				if (Settings::PlayerESP::BoneESP::bToggle)
					DrawPlayerBone(&Characters[i], teamIDColor);
			}
		}		
	}

	// Found a valid best target and Pawn is holding a valid firearm
	if (g_pAim->tmpNearestDist2Cross != 9999.0f && g_pAim->PawnBulletFireSpeed > 0.0f)
	{
		// Distance in game position between: Pawn head bone vs Enemy bone
		float distance{ Utils::DistBetween2Vector3D(g_pAim->tmpCharacter.GAME_BONE_HEAD, PawnHeadBoneGamePos) };
		float distanceMeter = distance / 100.0f;
		float zAssist = 1.0f;
		if (distanceMeter < 50.f)
			zAssist = 1.8f;
		else if (distanceMeter < 100.f)
			zAssist = 1.72f;
		else if (distanceMeter < 150.f)
			zAssist = 1.23f;
		else if (distanceMeter < 200.f)
			zAssist = 1.24f;
		else if (distanceMeter < 250.f)
			zAssist = 1.25f;
		else if (distanceMeter < 300.f)
			zAssist = 1.26f;
		else if (distanceMeter < 350.f)
			zAssist = 1.27f;
		else if (distanceMeter < 400.f)
			zAssist = 1.28f;
		else if (distanceMeter < 450.f)
			zAssist = 1.29f;
		else if (distanceMeter < 500.f)
			zAssist = 1.30f;

		// If distance is higher than 100000 so there is an error in tmpCharacter.GAME_BONE_HEAD or PawnHeadBoneGamePos
		if (distance < 100000)
		{
			// Time for bullet to reach the enemy position: t = S / v
			float BulletTravelTime{ distance / g_pAim->PawnBulletFireSpeed }; 

			// Get enemy velocity: v
			DWORD SceneComponent{ g_pMM->read<DWORD>(g_pAim->tmpCharacter.Address + 0x14C) };
			SDK::FVector ComponentVelocity{ g_pMM->read<SDK::FVector>(SceneComponent + 0x1B0) };

			// The distance that enemy moved 
			SDK::FVector PredictEnemyBonePos{ g_pAim->tmpCharacter.GAME_BONE_HEAD };
			PredictEnemyBonePos.X += ComponentVelocity.X * BulletTravelTime; // S = v * t
			PredictEnemyBonePos.Y += ComponentVelocity.Y * BulletTravelTime; // S = v * t
			PredictEnemyBonePos.Z += ComponentVelocity.Z * BulletTravelTime * zAssist + 0.5f * 5.72f * BulletTravelTime * BulletTravelTime; // S = v * t

			// Get predicted enemy bone position on screen
			// Don't need to check the return value of this function (is behind my player)
			// because it's already checked in the players for loop
			g_pVMM->GameToScreenBone(PredictEnemyBonePos, g_pAim->tmpTargetPos);

			// Line to best target
			g_pD3D->DrawLine((float)g_pD3D->screenW / 2, (float)g_pD3D->screenH, g_pAim->tmpCharacter.PositionOnSc.X, g_pAim->tmpCharacter.PositionOnSc.Y + g_pAim->tmpCharacter.PositionOnSc.Z + yOffset, RED(255));

			// Enemy predict movement line
			g_pD3D->DrawLine(g_pAim->tmpTargetPos.X, g_pAim->tmpTargetPos.Y, g_pAim->tmpCharacter.BONE_HEAD.X, g_pAim->tmpCharacter.BONE_HEAD.Y, WHITE(255), 1.5);

			// Dot at the end of enemy movement prediction line
			if (ComponentVelocity.X != 0 && ComponentVelocity.Y != 0)
				g_pD3D->DrawCircle(g_pAim->tmpTargetPos.X, g_pAim->tmpTargetPos.Y, (g_pAim->tmpCharacter.PositionOnSc.Z / 2) / 7, GRAY(255));
		}
		/*float BulletDrop(float TravelTime) {
			return (TravelTime * TravelTime * 980 / 2);
		}*/
		//Class: ShootWeaponEntity.WeaponEntity.WeaponLogicBaseComponent.ActorComponent.Object
	}

	g_pAim->GetTmpBestTarget();
	g_pAim->ResetTmpNearestTargetDist2Cross();
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
	for (int i = 0; i < Airdrops.size(); ++i)
	{
		if (!g_pVMM->WorldToScreen(Airdrops[i].Position, Airdrops[i].PositionOnSc, Airdrops[i].distance))
			continue;

		std::string txt{ "AirDrop " + std::to_string(Airdrops[i].distance) + 'm' };
		g_pD3D->DrawString(Airdrops[i].PositionOnSc.X, Airdrops[i].PositionOnSc.Y, RED(200), txt, true);
	}

	for (int i = 0; i < AirDropDatas.size(); ++i)
	{
		if (!g_pVMM->WorldToScreen(AirDropDatas[i].Position, AirDropDatas[i].PositionOnSc, AirDropDatas[i].distance))
			continue;

		if (Settings::bDebugESP)
		{
			/*g_pD3D->DrawString(AirDropDatas[i].PositionOnSc.X + 42, AirDropDatas[i].PositionOnSc.Y, RED(255), "Data", false);*/

			g_pD3D->DrawString(AirDropDatas[i].PositionOnSc.X, AirDropDatas[i].PositionOnSc.Y + 18, RED(255), Utils::DecToHex(Lootboxes[i].address).c_str(), false);

			g_pD3D->DrawString(AirDropDatas[i].PositionOnSc.X, AirDropDatas[i].PositionOnSc.Y + 18 + 18, RED(255), std::to_string(Lootboxes[i].itemCount).c_str(), false);
		}

		// AirDrop items
		if (AirDropDatas[i].items.size() == 0)
		{
			g_pD3D->DrawString(AirDropDatas[i].PositionOnSc.X, AirDropDatas[i].PositionOnSc.Y + 18, WHITE(200), "Empty", false);
		}
		else
		{
			int xOffset = 0;
			for (auto& itr : AirDropDatas[i].items)
			{
				g_pD3D->DrawString(AirDropDatas[i].PositionOnSc.X, AirDropDatas[i].PositionOnSc.Y + 18 + xOffset, WHITE(200), itr, false);
				xOffset += 18;
			}
		}
	}
}

void ESP::DrawLootbox()
{
	for (int i = 0; i < Lootboxes.size(); ++i)
	{
		if (!g_pVMM->WorldToScreen(Lootboxes[i].Position, Lootboxes[i].PositionOnSc, Lootboxes[i].distance))
			continue;

		if (Lootboxes[i].distance > Settings::LootboxESP::drawDistance)
			continue;	

		g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y, WHITE(255), "Lootbox", true);

		if (Settings::bDebugESP)
		{
			g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18, RED(255), Utils::DecToHex(Lootboxes[i].address).c_str(), false);

			g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18 + 18, RED(255), std::to_string(Lootboxes[i].itemCount).c_str(), false);

			std::string str = std::to_string(Lootboxes[i].PositionOnSc.X) + ' ' + std::to_string(Lootboxes[i].PositionOnSc.Y);
			g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18 + 18 + 18, RED(255), str, true);

			continue;
		}

		if (Lootboxes[i].items.size() == 0)
		{
			g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18, WHITE(200), "Empty", false);
		}
		else
		{
			int xOffset = 0;

			for (auto& itr : Lootboxes[i].items)
			{
				g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18 + xOffset, WHITE(200), itr, false);
				xOffset += 18;
			}
		}
	}
}

std::string ESP::GetActorName(DWORD actorID)
{
	DWORD FNamePtr{ g_pMM->read<DWORD>(Names + (actorID / 0x4000) * 4) };
	DWORD FName{ g_pMM->read<DWORD>(FNamePtr + 4 * (actorID % 0x4000)) };
	FName += 8;

	char name[70] {};

	// minus 1 : for null terminated. If not string will end with |||||||||||||||||| 
	g_pMM->readMemory((PVOID)FName, &name, sizeof(name)-1);

	std::string result = std::string(name);
	return result;
}

std::wstring ESP::GetPlayerName(DWORD nameAddr)
{
	TCHAR p[16]{};
	g_pMM->readMemory((PVOID)nameAddr, &p, sizeof(p) - 1);

	return std::wstring(p);
}

void ESP::DrawHeadBone(SDK::FVector2D headScreenPosition, float playerZ)
{
	g_pD3D->DrawCircle(headScreenPosition.X, headScreenPosition.Y, playerZ / 7, RED(255));
}

void ESP::DrawPlayerBone(Character* character, const unsigned int &color)
{
	DrawHeadBone(character->BONE_HEAD, character->PositionOnSc.Z / 2);

	g_pD3D->DrawLine(character->BONE_CHEST.X, character->BONE_CHEST.Y, character->BONE_PELVIS.X, character->BONE_PELVIS.Y, color, 1.2f);

	g_pD3D->DrawLine(character->BONE_CHEST.X, character->BONE_CHEST.Y, character->BONE_L_ELBOW.X, character->BONE_L_ELBOW.Y, color, 1.2f);
	g_pD3D->DrawLine(character->BONE_CHEST.X, character->BONE_CHEST.Y, character->BONE_R_ELBOW.X, character->BONE_R_ELBOW.Y, color, 1.2f);

	g_pD3D->DrawLine(character->BONE_L_ELBOW.X, character->BONE_L_ELBOW.Y, character->BONE_L_WRIST.X, character->BONE_L_WRIST.Y, color, 1.2f);
	g_pD3D->DrawLine(character->BONE_R_ELBOW.X, character->BONE_R_ELBOW.Y, character->BONE_R_WRIST.X, character->BONE_R_WRIST.Y, color, 1.2f);

	g_pD3D->DrawLine(character->BONE_PELVIS.X, character->BONE_PELVIS.Y, character->BONE_L_KNEE.X, character->BONE_L_KNEE.Y, color, 1.2f);
	g_pD3D->DrawLine(character->BONE_PELVIS.X, character->BONE_PELVIS.Y, character->BONE_R_KNEE.X, character->BONE_R_KNEE.Y, color, 1.2f);

	g_pD3D->DrawLine(character->BONE_L_KNEE.X, character->BONE_L_KNEE.Y, character->BONE_L_FOOT.X, character->BONE_L_FOOT.Y, color, 1.2f);
	g_pD3D->DrawLine(character->BONE_R_KNEE.X, character->BONE_R_KNEE.Y, character->BONE_R_FOOT.X, character->BONE_R_FOOT.Y, color, 1.2f);
}

bool ESP::GetPlayerBonePos(Character* character)
{
	DWORD SkeletalMeshComponent{ g_pMM->read<DWORD>(character->Address + MESH) };
	DWORD bodyAddr{ SkeletalMeshComponent + 0x150 };
	DWORD boneAddr{ g_pMM->read<DWORD>(SkeletalMeshComponent + 1456) + 48 };

	bool bBadBonePosition = false;

	// Iterate over BONES
	for (int curBone = 0; curBone < 11; ++curBone)
	{
		SDK::FVector curBoneGamePos{ g_pVMM->GetBoneGamePosition(bodyAddr, boneAddr + Bones[curBone] * 48) };
		switch (Bones[curBone])
		{
			case BONE_HEAD:
			{
				curBoneGamePos.Z += 5;
				character->GAME_BONE_HEAD = curBoneGamePos;
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_HEAD))
					bBadBonePosition = true;
				break;
			}
			case BONE_CHEST:
			{
				character->GAME_BONE_CHEST = curBoneGamePos;
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_CHEST))
					bBadBonePosition = true;
				break;
			}
			case BONE_PELVIS:
			{
				character->GAME_BONE_PELVIS = curBoneGamePos;
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_PELVIS))
					bBadBonePosition = true;
				break;
			}
			case BONE_L_ELBOW:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_L_ELBOW))
					bBadBonePosition = true;
				break;
			}
			case BONE_L_WRIST:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_L_WRIST))
					bBadBonePosition = true;
				break;
			}
			case BONE_R_ELBOW:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_R_ELBOW))
					bBadBonePosition = true;
				break;
			}
			case BONE_R_WRIST:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_R_WRIST))
					bBadBonePosition = true;
				break;
			}
			case BONE_L_KNEE:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_L_KNEE))
					bBadBonePosition = true;
				break;
			}
			case BONE_L_FOOT:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_L_FOOT))
					bBadBonePosition = true;
				break;
			}
			case BONE_R_KNEE:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_R_KNEE))
					bBadBonePosition = true;
				break;
			}
			case BONE_R_FOOT: {
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_R_FOOT))
					bBadBonePosition = true;
				break;
			}
		}

		if (bBadBonePosition)
			return false;
	}

	return true;
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
	if (actorName.find('/') != std::string::npos
		|| actorName.find("Parachute") != std::string::npos
		)
		return false;

	if (actorName.find("VH_") != std::string::npos
		||actorName.find("Mirado") != std::string::npos
		||actorName.find("water_Plane") != std::string::npos
		||actorName.find("AquaRail") != std::string::npos
		||actorName.find("Rony") != std::string::npos
		||actorName == "BP_AirDropPlane_C"
		)
		return true;

	return false;
}

bool ESP::IsPlayer(const std::string& actorName)
{
	if (actorName.find("PlayerCharacter") != std::string::npos
		|| actorName.find("FakePlayer") != std::string::npos
		|| actorName.find("PlayerPawn") != std::string::npos
		)
		return true;

	return false;
}

bool ESP::IsItem(const std::string& actorName)
{
	// Don't sort AirDrop and LootBox actor
	if (actorName == "PickUpListWrapperActor"
		|| actorName == "PickUpListWrapperActor_Recycled"
		|| actorName == "AirDropListWrapperActor"
		|| actorName == "AirDropListWrapperActor_Recycled"
		|| actorName == "BP_AirDropBox_C"
		|| actorName == "BP_AirDropBox_C_Recycled"
		)
		return false;

	if (actorName.find("Wrapper") == std::string::npos
		&& actorName.find("PickUp") == std::string::npos
		&& actorName.find("Pickup") == std::string::npos
		)
		return false;

	return true;
}

bool ESP::IsAirDropData(const std::string& actorName)
{
	if (actorName == "AirDropListWrapperActor" || actorName == "AirDropListWrapperActor_Recycled")
		return true;

	return false;
}

bool ESP::IsAirdrop(const std::string& actorName)
{
	if (actorName == "BP_AirDropBox_C" || actorName == "BP_AirDropBox_C_Recycled")
		return true;

	return false;
}

// Sometime Airdrop will have 2 actors: PlayerDeadInventoryBox_C and BP_AirDropBox_C
// So we dont use PlayerDeadInventoryBox_C to find Lootbox
// Instead, we will use PickUpListWrapperActor
bool ESP::IsLootbox(const std::string& actorName)
{
	if (actorName == "PickUpListWrapperActor" || actorName == "PickUpListWrapperActor_Recycled")
		return true;

	return false;
}

// TODO reformat void ESP::GetBoxItems(BoxData* boxData)
void ESP::GetBoxItems(BoxData* boxData)
{
	// Class: PickUpListWrapperActor.PickUpWrapperActor.UAENetActor.LuaActor.Actor.Object
	DWORD PickUpDataList{ g_pMM->read<DWORD>(boxData->address + PICKUPDATALIST) };
	int itemCount{ g_pMM->read<int>(boxData->address + PICKUPDATALIST + 0x4) }; // It's right before PickUpDataList

	// Copy pasta :D dont know why
	if (itemCount > 60)
		itemCount = 60;

	for (DWORD itemAddr = PickUpDataList; itemAddr < PickUpDataList + itemCount * 0x30; itemAddr += 0x30)
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
			if (Count == 0)
				continue;

			std::string txt{ BoxItemDisplayName[TypeSpecificID] };
			if (txt == "")
			{
				txt = std::to_string(TypeSpecificID);
				continue;
			}
			// Check if given string is not exist in items vector
			if (std::find(boxData->items.begin(), boxData->items.end(), txt) == boxData->items.end())
			{
				boxData->items.emplace_back(txt);
			}
			
			

			/*if (txt != "")
				boxData->items[txt] = Count + boxData->items[txt];
			else
				boxData->items[std::to_string(TypeSpecificID)] = Count + boxData->items[std::to_string(TypeSpecificID)];*/
			
			++boxData->itemCount;
		}
	}
}

