#include "pch.h"

#include "Offset.h"

const DWORD GWORLD = 4786820;
const DWORD GNAMES = 1924332;

// Class: World.Object
const DWORD NETDRIVER = 0x24;
const DWORD PERSISTENTLEVEL = 0x20;

// Class: NetDriver.Object
const DWORD SERVERCONNECTION = 0x60;

// Class: Player.Object
const DWORD PLAYERCONTROLLER = 0x20;

// Class: PlayerController.Controller.Actor.Object
const DWORD ACKNOWLEDGEDPAWN = 0x330;

// Class: UAECharacter.Character.Pawn.Actor.Object
const DWORD iREGIONCHARACTER = 0x6;
const DWORD TEAMID = 0x670;

// Class: Actor.Object
const DWORD ROOTCOMPONENT = 0x14C;

//Class: STExtraVehicleBase.Pawn.Actor.Object
const DWORD VEHICLECOMMON = 0x6a0;

//Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object
//uint64 CurrentStates;//[Offset: 0x920, Size: 8]

//Character.Pawn.Actor.Object //mesh, tmp
const DWORD MESH = 0x320; //[Offset: 0x320, Size: 4]

//Class: SceneComponent.ActorComponent.Object
// Bad, sometime not show enemy
const DWORD RELATIVELOCATION = 0x120; //Vector RelativeLocation; [Offset: 0x120, Size: 12]
//Good
const DWORD ACTORPOSITION = 0x160; 

// Class: PickUpListWrapperActor.PickUpWrapperActor.UAENetActor.LuaActor.Actor.Object
//PickUpItemData[] PickUpDataList;//[Offset: 0x640, Size: 12]
const DWORD PICKUPDATALIST = 0x640;

// Class: PickUpWrapperActor.UAENetActor.LuaActor.Actor.Object
const DWORD ITEMDEFINEID = 0x3a0; //ItemDefineID DefineID; [Offset: 0x3a0, Size: 24]

//enum BoxType;//[Offset: 0x658, Size: 1]