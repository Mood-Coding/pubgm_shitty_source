#include "pch.h"
#include "Settings.h"

namespace Settings
{
	bool bShowMenu = true;

	namespace PlayerESP
	{
		bool bToggle = true;

		namespace BoneESP
		{
			bool bToggle = true;
		}

		namespace LineESP
		{
			bool bToggle = false;
		}

		bool bDistance = true;
		bool bName = true;
		bool bHp = true;
	}

	namespace VehicleESP
	{
		bool bToggle = true;
		bool bDistance = true;
		bool bFuel = true;
		bool bHp = true;
		bool bName = true;
		int drawDistance = 1000;
	}

	namespace ItemESP
	{
		bool bToggle = true;
		bool bDistance = false;
		bool bName = true;
		int drawDistance = 18;
	}

	namespace LootboxESP
	{
		bool bToggle = true;
		int drawDistance = 50;
	}

	namespace AirDropESP
	{
		bool bToggle = true;
	}

	bool bOnlyShowBoxDataWhenMouseNearby = true;

	bool bUnsortedActorESP = false;
	bool bDebugESP = false;
	bool bSelfESP = false;

	namespace Aimbot
	{
		bool bToggle = true;
		int targetBone = BONE_HEAD;
		int sensitivity = 2;
		int delayBetweenEveryAimbotTime = 15;
	}

	bool bFightMode = false;

	int readMemloopDelay = 30;
	int drawLoopDelay = 16;
	int loopDelay = 20;

	bool bToggleShadowText = true;

	int tab = 1;
}