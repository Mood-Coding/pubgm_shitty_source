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

		namespace BoneESP
		{
			extern bool bToggle;
		}

		namespace LineESP
		{
			extern bool bToggle;
		}

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

	extern bool bUnsortedActorESP;
	extern bool bDebugESP;
	extern bool bSelfESP;

	namespace Aimbot
	{
		extern bool bToggle;
		extern int targetBone;
		extern int sensitivity;
		extern int delayBetweenEveryAimbotTime;
		extern const char* bone;
		extern int selectedBone;
	}

	extern bool bFightMode;


	extern int readMemloopDelay;
	extern int drawLoopDelay;

	extern bool bToggleShadowText;

	extern int activeTabID;
}