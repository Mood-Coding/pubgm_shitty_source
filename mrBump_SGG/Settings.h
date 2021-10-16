#pragma once
#include "ESP.h"

#define ESP_TAB_ID 1
#define AIMBOT_TAB_ID 2
#define SETTINGS_TAB_ID 3
#define DEBUG_TAB_ID 4

namespace Settings
{	
	extern bool bShowMenu;

	namespace PlayerESP
	{
		extern bool bToggle;

		extern bool bBoneToggle;
		
		extern bool bLineToggle;

		extern bool bDistance;
		extern bool bName;
		extern bool bHp;
	}

	namespace VehicleESP
	{
		extern bool bToggle;
		extern bool bDistance;
		extern bool bFuel;
		extern bool bHp;
		extern bool bName;
		extern int drawDistance;
	}

	namespace ItemESP
	{
		extern bool bToggle;
		extern bool bDistance;
		extern bool bName;
		extern int drawDistance;
	}

	namespace LootboxESP
	{
		extern bool bToggle;
		extern int drawDistance;
	}

	namespace AirDropESP
	{
		extern bool bToggle;
	}

	extern bool bOnlyShowBoxDataWhenMouseNearby;

	namespace Aimbot
	{
		extern bool bToggle;
		extern int sensitivity;
		extern int delayBetweenEveryAimbotTime;
		extern int fov;
		extern int selectedBone;

		extern int targetBone;
		extern const char* bone;
	}

	extern bool bFightMode;

	// Other
	extern int readMemloopDelay;
	extern int drawLoopDelay;
	extern bool bToggleShadowText;

	// Debug
	extern bool bUnsortedActorESP;
	extern bool bDebugESP;
	extern bool bSelfESP;

	extern int activeTabID;
}