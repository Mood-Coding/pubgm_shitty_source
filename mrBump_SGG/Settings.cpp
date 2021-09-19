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
			bool bToggle = true;
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
		bool bDistance = true;
		bool bName = true;
		int drawDistance = 9;
	}

	bool bUnsortedActorESP = false;

	namespace Aimbot
	{
		bool bToggle = true;
		int targetBone = BONE_HEAD;
	}

	int readMemloopDelay = 30;
	int drawLoopDelay = 16;
	int loopDelay = 10;

	bool bToggleShadowText = true;

	int tab = 1;
}