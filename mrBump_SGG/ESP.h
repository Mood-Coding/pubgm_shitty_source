#pragma once
#include"pch.h"

#include "Offset.h"	

#include "MemoryManager.h"
#include "Entity.h"
#include "ViewMatrixManager.h"
#include "SDK_classes.h"
#include "SDK_structs.h"
#include "Settings.h"
#include "ActorName.h"

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
	
	std::wstring PlayerName = L"";

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

	std::string displayName = "";
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

class Lootbox : public Actor
{
public:

};

struct AirDropItem
{
	std::string name{};
	int quantity = 0;
};

class Airdrop : public Actor
{
public:
	Airdrop(DWORD actorAddress, SDK::FVector actorPosition)
	{
		Address = actorAddress;
		Position = actorPosition;
	}

	Airdrop() {}

	std::unordered_map<std::string, std::string> Items;

	//std::string actorName{};
	int count = 0;
	
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

struct ActorCaching
{
public:
	ActorCaching() {}

	ActorCaching(std::string& actorName, std::string displayName, bool isItem)
	{
		ActorName = actorName;
		DisplayName = displayName;
		bIsItem = isItem;
	}

	ActorCaching(std::string& actorName, std::string displayName, std::wstring playerName)
	{
		ActorName = actorName;
		DisplayName = displayName;
	}

	std::string ActorName{};
	std::string DisplayName{};
	std::wstring PlayerName{};
	bool bIsItem = false;
};

class ESP
{
public:
	ESP() {}

	bool Init(std::wstring emulator);

	D3DMatrix viewMatrix;

	DWORD viewWorld = 0;

	DWORD GWorldOffset = 4786820;
	DWORD GNamesOffset = 1924332;

	// Class: World.Object
	DWORD NetDriverOffset = 0x24;
	DWORD PersistentLevel = 0x20;

	// Class: NetDriver.Object
	DWORD ServerConnectionOffset = 0x60;

	// Class: Player.Object
	DWORD PlayerControllerOffset = 0x20;

	// Class: PlayerController.Controller.Actor.Object
	DWORD AcknowledgedPawnOffset = 0x330;

	// Class: UAECharacter.Character.Pawn.Actor.Object
	DWORD iRegionCharacterOffset = 0x638;
	DWORD TeamIDOffset = 0x670;

	// Class: Actor.Object
	DWORD RootComponentOffset = 0x14C;

	//Class: STExtraVehicleBase.Pawn.Actor.Object
	DWORD VehicleCommonOffset = 0x6a0;

	//Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object
	//uint64 CurrentStates;//[Offset: 0x920, Size: 8]
	
	//Character.Pawn.Actor.Object //mesh, tmp
	DWORD MeshOffset = 0x320; //[Offset: 0x320, Size: 4]

	DWORD ActorPosOffset = 0x160;

	DWORD GWorld = 0;
	DWORD GNames = 0;
	
	DWORD Names = 0;
	//GNames -> Names -> Actor Name

	DWORD UWorld = 0;
	DWORD lastUWorld = 0; // For checking switch from game <-> lobby

	DWORD NetDriver = 0;
	
	DWORD Level = 0;
	

	DWORD ActorList = 0;
	DWORD ActorListOffset = 0x70; //112
	DWORD maxActorCount = 0;
	DWORD maxActorCountOffset = 0x78; //120

	DWORD SceneComponent = 0;

	std::vector<Vehicle> Vehicles;
	std::vector<Character> Characters;
	std::vector<Item> Items;
	std::vector<Airdrop> Airdrops;
	std::vector<Lootbox> Lootboxes;
	std::vector<UnsortedActor> UnsortedActors;

	std::map<DWORD, ActorCaching> ActorCache;
	
	int CharacterCount = 0;

	int MyTeamID = 0;
	

	// Pawn = current player acdress
	DWORD Pawn = 0;

	void DrawPlayers();
	void DrawItems();
	void DrawVehicles();
	void DrawAirDrop();
	void DrawUnsortedActors();

	std::string GetActorName(DWORD actorID);
	std::wstring GetPlayerName(DWORD nameAddr);

	void GetPlayerBonePos(Character* character);

	void DrawHeadBone(SDK::FVector2D headScreenPosition, int playerDistance);
	void DrawPlayerBone(Character* character);

	static int Bones[11];

	bool IsPlayer(const std::string& actorName);
	bool IsVehicle(const std::string& actorName);
	bool IsItem(const std::string& actorName, bool bIsItem, bool bIsCached);
	bool IsAirdrop(const std::string& actorName);
	bool IsLootbox(const std::string& actorName);
};

extern ESP* g_pESP;
#endif