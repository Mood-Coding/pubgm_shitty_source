#pragma once
#include"pch.h"

#include "Offset.h"	

#include "MemoryManager.h"
#include "ViewMatrixManager.h"
#include "SDK_classes.h"
#include "SDK_structs.h"
#include "Settings.h"
#include "ActorName.h"

#define MAX_ACTORS 1024

#ifndef ESP_H
#define ESP_H

enum BONES_ID
{
	BONE_HEAD = 5,

	BONE_CHEST = 4,
	BONE_PELVIS = 1,

	BONE_L_ELBOW = 12,
	BONE_L_WRIST = 13,

	BONE_R_ELBOW = 33,
	BONE_R_WRIST = 34,

	BONE_L_KNEE = 53,
	BONE_L_FOOT = 54,

	BONE_R_KNEE = 57,
	BONE_R_FOOT = 58,
};

class Actor
{
public:
	DWORD Address = 0;

	SDK::FVector Position;
	SDK::FVector PositionOnSc;

	int distance = 0;
};

class Character : public Actor
{
public:
	Character(DWORD actorAddress, SDK::FVector actorPosition)
	{
		Address = actorAddress;
		Position = actorPosition;
	}

	Character() {}
	
	SDK::STExtraCharacter STExtraCharacter{};
	
	std::wstring PlayerName{ L"" };

	SDK::FVector GAME_BONE_HEAD;
	SDK::FVector GAME_BONE_CHEST;
	SDK::FVector GAME_BONE_PELVIS;

	SDK::FVector2D BONE_HEAD;
	SDK::FVector2D BONE_CHEST;
	SDK::FVector2D BONE_PELVIS;
	SDK::FVector2D BONE_L_ELBOW;
	SDK::FVector2D BONE_L_WRIST;
	SDK::FVector2D BONE_R_ELBOW;
	SDK::FVector2D BONE_R_WRIST;
	SDK::FVector2D BONE_L_KNEE;
	SDK::FVector2D BONE_L_FOOT;
	SDK::FVector2D BONE_R_KNEE;
	SDK::FVector2D BONE_R_FOOT;

	unsigned int TeamColor{ 0 };
	int TeamIDIndex{ 0 };
};

class Item : public Actor
{
public:
	Item(DWORD actorAddress, SDK::FVector actorPosition)
	{
		Address = actorAddress;
		Position = actorPosition;
	}

	Item() {}

	std::string displayName{ "" };

	// Only for DebugESP
	std::string actorName{ "" };
	SDK::ItemDefineID ItemDefineID{};
};

class Vehicle : public Actor
{
public:
	Vehicle(DWORD actorAddress, SDK::FVector actorPosition)
	{
		Address = actorAddress;
		Position = actorPosition;
	}

	Vehicle() {}

	SDK::VehicleCommonComponent VehicleCommonComponent{};
	std::string displayName = "";
};


struct AirDropItem
{
	std::string name{};
	int quantity = 0;
};

struct BoxData
{
public:
	BoxData(std::string& actorName, DWORD currActorAddr, SDK::FVector actorPosition)
	{
		ActorName = actorName;
		address = currActorAddr;
		Position = actorPosition;
	}

	BoxData() {}

	std::string ActorName;
	DWORD address = 0;
	SDK::FVector Position;
	SDK::FVector PositionOnSc;

	int distance = 0; 

	//std::unordered_map<std::string, int> items;
	std::vector<std::string> items;
	//int itemCount = 0;
};

class Airdrop : public Actor
{
public:
	Airdrop(DWORD actorAddress, SDK::FVector actorPosition)
	{
		Address = actorAddress;
		Position = actorPosition;
	}

	BoxData boxData;
};

class Lootbox : public Actor
{
public:
	Lootbox(DWORD actorAddress, SDK::FVector actorPosition)
	{
		Address = actorAddress;
		Position = actorPosition;
	}

	BoxData boxData;
};

class UnsortedActor : public Actor
{
public:
	UnsortedActor(std::string& actorName, SDK::FVector actorPosition)
	{
		Position = actorPosition;
		ActorName = actorName;
	}

	std::string ActorName{};
};

class ESP
{
public:
	ESP() {}

	bool Init();

	D3DMatrix viewMatrix;

	DWORD viewWorld = 0;

	DWORD GWorld = 0;
	DWORD GNames = 0;
	DWORD Names = 0;
	//GNames -> Names -> Actor Name

	DWORD UWorld = 0;

	DWORD NetDriver = 0;
	
	DWORD Level = 0;

	DWORD ActorList = 0;
	DWORD ActorListOffset = 0x70; //112
	DWORD maxActorCount = 0;
	DWORD maxActorCountOffset = 0x78; //120

	DWORD SceneComponent = 0;

	// Current player acdress
	DWORD Pawn = 0;
	SDK::FVector PawnHeadBoneGamePos{};

	DWORD viewMatrixAddr = 0;

	int MyTeamID = 0;

	int CharacterCount = 0;

	std::vector<Vehicle> Vehicles;
	std::vector<Character> Characters;
	std::vector<Item> Items;
	std::vector<Airdrop> Airdrops;
	std::vector<BoxData> Lootboxes;
	std::vector<UnsortedActor> UnsortedActors;
	std::vector<BoxData> AirDropDatas;

	// ActorID -> ActorName
	std::unordered_map<DWORD, std::string> ActorNameCache;	
	
	std::vector<int> TeamIDIndex;

	void DrawPlayers();
	void DrawItems();
	void DrawVehicles();
	void DrawAirDrop();
	void DrawLootbox();
	void DrawUnsortedActors();

	void GetBoxItems(BoxData* boxData);

	std::string GetActorName(const DWORD& actorID);
	std::wstring GetPlayerName(const DWORD& nameAddr);

	bool GetPlayerBonePos(Character* character);

	static int Bones[11];

	void DrawHeadBone(const SDK::FVector2D& headScreenPosition, float playerZ);
	void DrawPlayerBone(Character* character, const unsigned int& color);

	bool IsPlayer(const std::string& actorName);
	bool IsVehicle(const std::string& actorName);
	bool IsItem(const std::string& actorName);
	bool IsAirdrop(const std::string& actorName);
	bool IsAirDropData(const std::string& actorName);
	bool IsLootbox(const std::string& actorName);
};

extern ESP* g_pESP;
#endif