#pragma once
#include "datatype.h"

#ifndef ENTITY_H
#define ENTITY_H
//class Entity {
//public:	
//	Entity() {
//	}
//
//	~Entity() {
//	}
//
//	//sf::RectangleShape box;
//
//	//all entities has these in4
//	DWORD entityAddr = 0;
//	DWORD entityWorld = 0;
//	DWORD entityStruct = 0;
//	DWORD entityStatus = 0;
//
//	std::string entityClassName = "";
//	std::string entityDisplayName = "";
//
//	Vector3f position;
//	Vector3f positionOnScreen;
//	int distanceMeter = 0;
//
//	//only player entity has these in4
//	//trạng thái người chơi:
//	//đứng, đi bộ, bơi 0
//	//ngồi 1
//	//ngồi + sprint = 4
//	//nằm 2
//	//sprint, bơi + sprint = 3
//	//nằm + sprint = 5
//	DWORD pose = 0;
//	DWORD teamID = 0;
//	DWORD health = 0;
//	std::wstring name = L"";
//
//	Vector2f BONE_HEAD;
//	Vector2f BONE_CHEST;
//	Vector2f BONE_PELVIS;
//	Vector2f BONE_L_ELBOW;
//	Vector2f BONE_L_WRIST;
//	Vector2f BONE_R_ELBOW;
//	Vector2f BONE_R_WRIST;
//	Vector2f BONE_L_KNEE;
//	Vector2f BONE_L_FOOT;
//	Vector2f BONE_R_KNEE;
//	Vector2f BONE_R_FOOT;
//	
//};

//class Actor
//{
//public:
//	int iRegionCharacter;//[Offset: 0x634, Size: 4]
//	FName PlayerType;//[Offset: 0x640, Size: 8]
//	FString PlayerName;//[Offset: 0x648, Size: 12]
//	FString Nation;//[Offset: 0x654, Size: 12]
//	uint32 PlayerKey;//[Offset: 0x660, Size: 4]
//	FString PlayerUID;//[Offset: 0x664, Size: 12]
//	int TeamID;//[Offset: 0x670, Size: 4]
//	bool bTeamLeader;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x674, Size: 1]
//	int CampID;//[Offset: 0x678, Size: 4]
//	int resID;//[Offset: 0x67c, Size: 4]
//	enum DefaultCharacterGender;//[Offset: 0x680, Size: 1]
//	GameModePlayerItem[] InitialItemList;//[Offset: 0x684, Size: 12]
//	GameModePlayerUpassInfo UpassInfo;//[Offset: 0x690, Size: 44]
//	GameModePlayerPetInfo PetInfo;//[Offset: 0x6bc, Size: 24]
//	int planeAvatarId;//[Offset: 0x6d4, Size: 4]
//	int DyeDebugFlag;//[Offset: 0x6d8, Size: 4]
//	GameModePlayerTaskData[] InitialTaskDataList;//[Offset: 0x6dc, Size: 12]
//	bool bIsAI;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x6e8, Size: 1]
//	bool bIsAIWithPet;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x6e9, Size: 1]
//	bool UseWholeBodyModel;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x700, Size: 1]
//	bool DefaultIsUseWholeBodyModel;//(ByteOffset: 0, ByteMask: 1, FieldMask: 255)[Offset: 0x701, Size: 1]
//	FString LuaFilePath;//[Offset: 0x710, Size: 12]
//	LuaNetSerialization LuaNetSerialization;//[Offset: 0x71c, Size: 60]
//}

#endif