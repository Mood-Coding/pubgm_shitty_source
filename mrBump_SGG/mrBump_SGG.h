#pragma once

#include "pch.h"

#include "Offset.h"
#include "D3D.h"
#include "ESP.h"
#include "Aimbot.h"
#include "Settings.h"
#include "MemoryManager.h"
#include "ProcessManager.h"

std::vector<Character> tmpCharacters;
std::vector<Vehicle> tmpVehicles;
std::vector<Item> tmpItems;
std::vector<Airdrop> tmpAirDrops;
std::vector<BoxData> tmpLootboxes;
std::vector<BoxData> tmpAirDropDatas;

std::vector<UnsortedActor> tmpUnsortedActors;

extern bool g_bActive;
