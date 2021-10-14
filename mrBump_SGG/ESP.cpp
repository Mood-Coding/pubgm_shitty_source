#include "pch.h"

#include "ESP.h"
#include "Aimbot.h"

ESP* g_pESP = new ESP();

bool ESP::Init()
{
	std::cout << "[ESP]\n";

	viewWorld = g_pVMM->GetViewWorld(g_pMM->emuProcName);
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

//////////////////////////
// DRAW ACTOR FUNCTIONS //
//////////////////////////

void ESP::DrawItems()
{
	for (int i{ 0 }; i < Items.size(); ++i)
	{	
		if (!g_pVMM->WorldToScreen(Items[i].Position, Items[i].PositionOnSc, Items[i].distance))
			continue;

		if (Items[i].distance > Settings::ItemESP::drawDistance)
			continue;

		float distScale{ g_pD3D->max_text_size - Items[i].distance * 0.01f };

		if (Settings::bDebugESP)
		{
			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y, WHITE(255), Utils::DecToHex<DWORD>(Items[i].Address).c_str(), distScale, Settings::bToggleShadowText);
			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y + 18, WHITE(255), Items[i].actorName, distScale, Settings::bToggleShadowText);
			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y + 18 + 18, WHITE(255), Items[i].displayName, distScale, Settings::bToggleShadowText);
			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y + 18 + 18 + 18, WHITE(255), std::to_string(Items[i].ItemDefineID.TypeSpecificID).c_str(), distScale, Settings::bToggleShadowText);
			std::string txt{ std::to_string(Items[i].PositionOnSc.X) + ' ' + std::to_string(Items[i].PositionOnSc.Y) };
			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y + 18 + 18 + 18 + 18, WHITE(255), txt.c_str(), distScale, Settings::bToggleShadowText);

			continue;
		}

		unsigned int itemDisplayColor{ ColorFilter[ActorColorFilterID[Items[i].displayName]] };
		if (!itemDisplayColor)
			itemDisplayColor = DARKRED(255);

		std::string str;

		if (Settings::ItemESP::bName)
			str += Items[i].displayName + ' ';

		if (Settings::ItemESP::bDistance)
			str += std::to_string(Items[i].distance) + "m";

		if (str != "")
			g_pD3D->DrawString(Items[i].PositionOnSc.X, Items[i].PositionOnSc.Y, itemDisplayColor, str, distScale, Settings::bToggleShadowText);
	}
}

void ESP::DrawVehicles()
{
	for (int i{ 0 }; i < Vehicles.size(); ++i)
	{
		if (!g_pVMM->WorldToScreen(Vehicles[i].Position, Vehicles[i].PositionOnSc, Vehicles[i].distance))
			continue;

		//TODO move BP_AirDrop_Plane to special actor vector
		/*if (Vehicles[i].displayName == "AirDrop Plane")
		{
			g_pD3D->DrawString(Vehicles[i].PositionOnSc.X, Vehicles[i].PositionOnSc.Y, LAWNGREEN(255), "AirDrop Plane", distScale, Settings::bToggleShadowText);
			continue;
		}*/

		if (Vehicles[i].distance > Settings::VehicleESP::drawDistance)
			continue;

		float distScale{ g_pD3D->max_text_size - Vehicles[i].distance * 0.0012f };

		if (Settings::bDebugESP)
		{
			// Address
			g_pD3D->DrawString(Vehicles[i].PositionOnSc.X, Vehicles[i].PositionOnSc.Y, WHITE(255), Utils::DecToHex<DWORD>(Vehicles[i].Address).c_str(), distScale, Settings::bToggleShadowText);

			// HP
			std::string str{ std::to_string((int)Vehicles[i].VehicleCommonComponent.HP) + '/' + std::to_string((int)Vehicles[i].VehicleCommonComponent.HPMax) };
			g_pD3D->DrawString(Vehicles[i].PositionOnSc.X, Vehicles[i].PositionOnSc.Y + 18, WHITE(255), str.c_str(), distScale, Settings::bToggleShadowText);

			// Fuel
			str = std::to_string((int)Vehicles[i].VehicleCommonComponent.Fuel) + '/' + std::to_string((int)Vehicles[i].VehicleCommonComponent.FuelMax);
			g_pD3D->DrawString(Vehicles[i].PositionOnSc.X, Vehicles[i].PositionOnSc.Y + 18 + 18, WHITE(255), str.c_str(), distScale, Settings::bToggleShadowText);

			continue;
		}

		std::string str{};

		// vehicle name + distance
		{
			if (Settings::VehicleESP::bName)
				str = Vehicles[i].displayName + ' ';

			if (Settings::VehicleESP::bDistance)
				str += std::to_string(Vehicles[i].distance) + 'm';

			if (str != "")
				g_pD3D->DrawString(Vehicles[i].PositionOnSc.X, Vehicles[i].PositionOnSc.Y, LAWNGREEN(255), str, distScale, Settings::bToggleShadowText);
		}

		// %hp + %fuel
		{
			float xOffset = 0;

			if (Settings::VehicleESP::bHp)
			{
				int hpPercent{ (int)(Vehicles[i].VehicleCommonComponent.HP / Vehicles[i].VehicleCommonComponent.HPMax * 100) };
				str = "HP:" + std::to_string(hpPercent) + '%';

				g_pD3D->DrawString(Vehicles[i].PositionOnSc.X, Vehicles[i].PositionOnSc.Y + 15, ImColor::HSV( (float)hpPercent / 360 , 1.0f, 1.0f), str, distScale, Settings::bToggleShadowText);

				// Fuel text will be next to Hp text
				xOffset += floor(g_pD3D->font->CalcTextSizeA(distScale, FLT_MAX, -1.0f, str.c_str(), NULL, NULL).x) + 5.0f;
			}

			if (Settings::VehicleESP::bFuel)
			{
				int fuelPercent{ (int)(Vehicles[i].VehicleCommonComponent.Fuel / Vehicles[i].VehicleCommonComponent.FuelMax * 100) };
				str = "Fuel:" + std::to_string(fuelPercent) + '%';

				g_pD3D->DrawString(Vehicles[i].PositionOnSc.X + xOffset, Vehicles[i].PositionOnSc.Y + 15, ImColor::HSV( (float)fuelPercent / 360, 1.0f, 1.0f), str, distScale, Settings::bToggleShadowText);
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

			float distScale{ g_pD3D->max_text_size - Characters[i].distance * 0.01f };

			g_pD3D->DrawString(Characters[i].PositionOnSc.X, Characters[i].PositionOnSc.Y, WHITE(255), Utils::DecToHex<DWORD>(Characters[i].Address).c_str(), distScale, Settings::bToggleShadowText);
			g_pD3D->DrawString(Characters[i].PositionOnSc.X, Characters[i].PositionOnSc.Y + 18, WHITE(255), std::to_string(Characters[i].TeamIDIndex).c_str(), distScale, Settings::bToggleShadowText);
		}

		return;
	}

	int yOffset{ 0 };

	for (int i = 0; i < Characters.size(); ++i)
	{
		++CharacterCount;

		if (!g_pVMM->WorldToScreenPlayer(Characters[i].Position, Characters[i].PositionOnSc, Characters[i].distance))
			continue;

		float distScale{ g_pD3D->max_text_size - Characters[i].distance * 0.01f };

		yOffset = 0;

		unsigned int teamIDColor;

		if (Characters[i].STExtraCharacter.bIsAI)
			teamIDColor = CYAN(255);
		else
			teamIDColor = Characters[i].TeamColor;


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
				//ImVec2 size{ ImGui::CalcTextSize(str.c_str()) };
				ImVec2 size{ g_pD3D->font->CalcTextSizeA(distScale, FLT_MAX, -1.0f, str.c_str(), NULL, NULL) };
				g_pD3D->DrawString(Characters[i].PositionOnSc.X - floor(size.x / 2), Characters[i].PositionOnSc.Y + Characters[i].PositionOnSc.Z + yOffset, WHITE(255), str, distScale, Settings::bToggleShadowText);

				yOffset += 18;
			}
		}

		// Knocked check (hp <= 0)
		if (Characters[i].STExtraCharacter.Health <= 0)
		{
			g_pD3D->DrawString(Characters[i].PositionOnSc.X - (float)floor(53 / 2), Characters[i].PositionOnSc.Y + Characters[i].PositionOnSc.Z + yOffset, RED(255), "Knocked", distScale, Settings::bToggleShadowText);

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
	if (g_pAim->NearestDist2Cross != 9999.0f && g_pAim->PawnBulletFireSpeed > 0.0f)
	{
		// Distance in game position between: Pawn head bone vs Enemy bone
		float distance{ Utils::DistBetween2Vector3D(PawnHeadBoneGamePos, g_pAim->TargetGamePos) };
		float zAssist{ 1.0f };
		
		if (distance < 5000.f)
			zAssist = 1.8f;
		else if (distance < 10000.f)
			zAssist = 1.72f;
		else if (distance < 15000.f)
			zAssist = 1.23f;
		else if (distance < 20000.f)
			zAssist = 1.24f;
		else if (distance < 25000.f)
			zAssist = 1.25f;
		else if (distance < 30000.f)
			zAssist = 1.26f;
		else if (distance < 35000.f)
			zAssist = 1.27f;
		else if (distance < 40000.f)
			zAssist = 1.28f;
		else if (distance < 45000.f)
			zAssist = 1.29f;
		else if (distance < 50000.f)
			zAssist = 1.30f;
		else zAssist = 1.35f;

		// If distance is higher than 80000 so there is an error in tmpCharacter.GAME_BONE_HEAD or PawnHeadBoneGamePos
		if (distance < 80000.0f)
		{
			// Time for bullet to reach the enemy position: t = S / v
			float BulletTravelTime{ distance / g_pAim->PawnBulletFireSpeed }; 

			// Get enemy velocity: v
			DWORD SceneComponent{ g_pMM->read<DWORD>(g_pAim->TargetCharacter.Address + 0x14C) };
			SDK::FVector ComponentVelocity{ g_pMM->read<SDK::FVector>(SceneComponent + 0x1B0) };

			// The distance that enemy will move in BulletTravelTime duration
			SDK::FVector PredictedEnemyGamePos{ g_pAim->TargetGamePos };
			if (PredictedEnemyGamePos.X != 0 && PredictedEnemyGamePos.Y != 0 && PredictedEnemyGamePos.Z != 0)
			{
				PredictedEnemyGamePos.X += ComponentVelocity.X * BulletTravelTime; // S = v * t
				PredictedEnemyGamePos.Y += ComponentVelocity.Y * BulletTravelTime; // S = v * t
				PredictedEnemyGamePos.Z += ComponentVelocity.Z * BulletTravelTime * zAssist + 0.5f * g_pAim->GRAVITY * BulletTravelTime * BulletTravelTime /** 5.0f*/; // S = v * t
			}
			else
			{
				//PredictedEnemyGamePos.Z *= zAssist;
			}

			// Get predicted enemy bone position on screen
			g_pVMM->GameToScreenBone(PredictedEnemyGamePos, g_pAim->TargetScPredictedPos);

			// Line to best target
			g_pD3D->DrawLine((float)g_pD3D->screenW / 2, (float)g_pD3D->screenH, g_pAim->TargetCharacter.PositionOnSc.X, g_pAim->TargetCharacter.PositionOnSc.Y + g_pAim->TargetCharacter.PositionOnSc.Z + yOffset, RED(255));

			// Enemy predict movement line
			g_pD3D->DrawLine(g_pAim->TargetScPos.X, g_pAim->TargetScPos.Y, g_pAim->TargetScPredictedPos.X, g_pAim->TargetScPredictedPos.Y, WHITE(255), 1.5);

			// Dot at the end of enemy movement prediction line
			if (ComponentVelocity.X != 0 && ComponentVelocity.Y != 0)
				g_pD3D->DrawCircle(g_pAim->TargetScPredictedPos.X, g_pAim->TargetScPredictedPos.Y, (g_pAim->TargetCharacter.PositionOnSc.Z / 2) / 7, WHITE(250));
		}

		//Class: ShootWeaponEntity.WeaponEntity.WeaponLogicBaseComponent.ActorComponent.Object
	}

	g_pAim->GetTmpBestTarget();
	g_pAim->ResetTmpNearestTargetDist2Cross();
}

void ESP::DrawAirDrop()
{
	for (int i{ 0 }; i < Airdrops.size(); ++i)
	{
		if (!g_pVMM->WorldToScreen(Airdrops[i].Position, Airdrops[i].PositionOnSc, Airdrops[i].distance))
			continue;

		float distScale{ g_pD3D->max_text_size - Airdrops[i].distance * 0.001f };

		// Airdrop text + distance
		std::string txt{ "AirDrop " + std::to_string(Airdrops[i].distance) + 'm' };

		g_pD3D->DrawString(Airdrops[i].PositionOnSc.X, Airdrops[i].PositionOnSc.Y, RED(250), txt, distScale, true);
	}

	for (int i{ 0 }; i < AirDropDatas.size(); ++i)
	{
		if (!g_pVMM->WorldToScreen(AirDropDatas[i].Position, AirDropDatas[i].PositionOnSc, AirDropDatas[i].distance))
			continue;

		float distScale{ g_pD3D->max_text_size - AirDropDatas[i].distance * 0.001f };

		if (Settings::bDebugESP)
		{
			// AirDropData address
			g_pD3D->DrawString(AirDropDatas[i].PositionOnSc.X, AirDropDatas[i].PositionOnSc.Y + 18, RED(255), Utils::DecToHex(AirDropDatas[i].address).c_str(), distScale, false);

			// AirDropData item count
			g_pD3D->DrawString(AirDropDatas[i].PositionOnSc.X, AirDropDatas[i].PositionOnSc.Y + 18 + 18, RED(255), std::to_string(AirDropDatas[i].items.size()).c_str(), distScale, false);

			continue;
		}

		// AirDrop items
		if (AirDropDatas[i].items.size() > 0)
		{
			float yOffset{ 0.0f };
			for (const auto& itr : AirDropDatas[i].items)
			{
				g_pD3D->DrawString(AirDropDatas[i].PositionOnSc.X, AirDropDatas[i].PositionOnSc.Y + 18 + yOffset, WHITE(255), itr, distScale, false);
				yOffset += 18.0f;
			}
		}
		else
		{
			g_pD3D->DrawString(AirDropDatas[i].PositionOnSc.X, AirDropDatas[i].PositionOnSc.Y + 18, WHITE(255), "Empty", distScale, false);
		}
	}
}

void ESP::DrawLootbox()
{
	for (int i{ 0 }; i < Lootboxes.size(); ++i)
	{
		if (!g_pVMM->WorldToScreen(Lootboxes[i].Position, Lootboxes[i].PositionOnSc, Lootboxes[i].distance))
			continue;

		if (Lootboxes[i].distance > Settings::LootboxESP::drawDistance)
			continue;	

		float distScale{ g_pD3D->max_text_size - Lootboxes[i].distance * 0.01f };

		if (Settings::bDebugESP)
		{
			// Lootbox address
			g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18, WHITE(255), Utils::DecToHex(Lootboxes[i].address).c_str(), distScale, false);

			// Lootbox item count
			g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18 + 18, WHITE(255), std::to_string(Lootboxes[i].items.size()).c_str(), distScale, false);

			// Lootbox pos
			std::string str{ std::to_string(Lootboxes[i].PositionOnSc.X) + ' ' + std::to_string(Lootboxes[i].PositionOnSc.Y) };
			g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18 + 18 + 18, WHITE(255), str, distScale, false);

			continue;
		}

		// Lootbox text
		g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y, WHITE(255), "Lootbox", distScale, true);

		// Lootbox item
		if (Lootboxes[i].items.size() > 0)
		{
			float yOffset{ 0.0f };
			for (const auto& itr : Lootboxes[i].items)
			{
				g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18 + yOffset, WHITE(255), itr, distScale, false);
				yOffset += 18.0f;
			}
		}
		else
		{
			g_pD3D->DrawString(Lootboxes[i].PositionOnSc.X, Lootboxes[i].PositionOnSc.Y + 18, WHITE(255), "Empty", distScale, false);
		}
	}
}

void ESP::DrawUnsortedActors()
{
	for (int i{ 0 }; i < UnsortedActors.size(); ++i)
	{
		if (!g_pVMM->WorldToScreen(UnsortedActors[i].Position, UnsortedActors[i].PositionOnSc, UnsortedActors[i].distance))
			continue;

		g_pD3D->DrawString(UnsortedActors[i].PositionOnSc.X, UnsortedActors[i].PositionOnSc.Y, PINK(255), UnsortedActors[i].ActorName, g_pD3D->max_text_size, false);
	}
}

void ESP::DrawHeadBone(const SDK::FVector2D& headScreenPosition, float playerZ)
{
	g_pD3D->DrawCircle(headScreenPosition.X, headScreenPosition.Y, playerZ / 7, RED(255));
}

void ESP::DrawPlayerBone(Character* character, const unsigned int& color)
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

//////////////////////////////
// END DRAW ACTOR FUNCTIONS //
//////////////////////////////

std::string ESP::GetActorName(const DWORD& actorID)
{
	DWORD FNamePtr{ g_pMM->read<DWORD>(Names + (actorID / 0x4000) * 4) };
	DWORD FName{ g_pMM->read<DWORD>(FNamePtr + 4 * (actorID % 0x4000)) };
	FName += 8;

	char name[70] {};

	// minus 1 : for null terminated. If not string will end with |||||||||||||||||| 
	g_pMM->readMemory((PVOID)FName, &name, sizeof(name)-1);

	return std::string(name);
}

std::wstring ESP::GetPlayerName(const DWORD& nameAddr)
{
	TCHAR p[16]{};
	g_pMM->readMemory((PVOID)nameAddr, &p, sizeof(p) - 1);

	return std::wstring(p);
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

bool ESP::GetPlayerBonePos(Character* character)
{
	DWORD SkeletalMeshComponent{ g_pMM->read<DWORD>(character->Address + MESH) };
	DWORD bodyAddr{ SkeletalMeshComponent + 0x150 };
	DWORD boneAddr{ g_pMM->read<DWORD>(SkeletalMeshComponent + 1456) + 48 };

	// Iterate over BONES
	for (int curBone{ 0 }; curBone < 11; ++curBone)
	{
		SDK::FVector curBoneGamePos{ g_pVMM->GetBoneGamePosition(bodyAddr, boneAddr + Bones[curBone] * 48) };
		switch (Bones[curBone])
		{
			case BONE_HEAD:
			{
				curBoneGamePos.Z += 5;
				character->GAME_BONE_HEAD = curBoneGamePos;
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_HEAD))
					return false;
				break;
			}
			case BONE_CHEST:
			{
				character->GAME_BONE_CHEST = curBoneGamePos;
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_CHEST))
					return false;
				break;
			}
			case BONE_PELVIS:
			{
				character->GAME_BONE_PELVIS = curBoneGamePos;
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_PELVIS))
					return false;
				break;
			}
			case BONE_L_ELBOW:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_L_ELBOW))
					return false;
				break;
			}
			case BONE_L_WRIST:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_L_WRIST))
					return false;
				break;
			}
			case BONE_R_ELBOW:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_R_ELBOW))
					return false;
				break;
			}
			case BONE_R_WRIST:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_R_WRIST))
					return false;
				break;
			}
			case BONE_L_KNEE:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_L_KNEE))
					return false;
				break;
			}
			case BONE_L_FOOT:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_L_FOOT))
					return false;
				break;
			}
			case BONE_R_KNEE:
			{
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_R_KNEE))
					return false;
				break;
			}
			case BONE_R_FOOT: {
				if (!g_pVMM->GameToScreenBone(curBoneGamePos, character->BONE_R_FOOT))
					return false;
				break;
			}
		}
	}

	return true;
}

void ESP::GetBoxItems(BoxData* boxData)
{
	SDK::PickUpListWrapperActor PickUpList{ g_pMM->read<SDK::PickUpListWrapperActor>(boxData->address) };

	for (DWORD itemAddr{ PickUpList.PickUpDataList }; itemAddr < PickUpList.PickUpDataList + (int)PickUpList.PickUpItemDataCount * sizeof(SDK::PickUpItemData); itemAddr += sizeof(SDK::PickUpItemData))
	{
		SDK::PickUpItemData ItemData{ g_pMM->read<SDK::PickUpItemData>(itemAddr) };

		if (ItemData.Count == 0)
			continue;

		std::string item_display_name = BoxItemDisplayName[ItemData.TypeSpecificID];
		
		// Current item isn't exist in the list of desired items to appear
		if (item_display_name == "") continue;

		// If Item display name is not exist in Item display name list so add it
		if (std::find(boxData->items.begin(), boxData->items.end(), item_display_name) == boxData->items.end())
			boxData->items.emplace_back(item_display_name);
	}
}

//////////////////////
// CHECK ACTOR TYPE //
//////////////////////

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

// Sometime both LootBox and AirDrop will have actor: PlayerDeadInventoryBox_C
// Instead using PlayerDeadInventoryBox_C to recognise Lootbox, we use PickUpListWrapperActor
bool ESP::IsLootbox(const std::string& actorName)
{
	if (actorName == "PickUpListWrapperActor" || actorName == "PickUpListWrapperActor_Recycled")
		return true;

	return false;
}

//////////////////////////
// END CHECK ACTOR TYPE //
//////////////////////////