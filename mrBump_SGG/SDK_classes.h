#pragma once
#include "SDK_structs.h"

#ifndef _SDK_CLASSES_H_
#define _SDK_CLASSES_H_

namespace SDK
{
	// Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object
	class STExtraCharacter
	{
	public:
		// Class: UAECharacter.Character.Pawn.Actor.Object
		unsigned char Padding69[0x648];
		DWORD PlayerName; //[Offset: 0x648, Size: 12] FString PlayerName;

		unsigned char Padding59[0x24];
		int TeamID; //[Offset: 0x670, Size: 4]

		unsigned char Padding01[0x74];
		bool bIsAI;//[Offset: 0x6e8, Size: 1]

		// Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object
		unsigned char Padding00[0x23F];
		float Health; //[Offset: 0x928, Size: 4]
		float HealthMax; //[Offset: 0x92c, Size: 4]
	};

	// Class: VehicleCommonComponent.VehicleComponent.ActorComponent.Object
	class VehicleCommonComponent
	{
	public:
		unsigned char Padding00[0x1b0];
		float HPMax; //[Offset: 0x1b0, Size: 4]
		float HP; //[Offset: 0x1b4, Size: 4]

		unsigned char Padding01[0x10];
		float FuelMax; //[Offset: 0x1c8, Size: 4]
		float Fuel; //[Offset: 0x1cc, Size: 4]
	};

	struct ItemDefineID
	{
		int32_t Type; //0x0000
		int32_t TypeSpecificID; //0x0004
		bool bValidItem; //0x0008
		bool bValidInstance; //0x0009
		uint64_t InstanceID; //0x000A
	}; //Size: 0x0012


}

#endif // !_SDK_CLASSES_H_
