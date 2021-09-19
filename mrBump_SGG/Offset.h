#pragma once

#include "pch.h"

#ifndef _OFFSET_H_
#define _OFFSET_H_

extern const DWORD GWORLD;
extern const DWORD GNAMES;

// Class: World.Object
extern const DWORD NETDRIVER;
extern const DWORD PERSISTENTLEVEL;

// Class: NetDriver.Object
extern const DWORD SERVERCONNECTION;

// Class: Player.Object
extern const DWORD PLAYERCONTROLLER;

// Class: PlayerController.Controller.Actor.Object
extern const DWORD ACKNOWLEDGEDPAWN;

// Class: UAECharacter.Character.Pawn.Actor.Object
extern const DWORD iREGIONCHARACTER;
extern const DWORD TEAMID;

// Class: Actor.Object
extern const DWORD ROOTCOMPONENT;

//Class: STExtraVehicleBase.Pawn.Actor.Object
extern const DWORD VEHICLECOMMON;

//Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object
//uint64 CurrentStates;//[Offset: 0x920, Size: 8]

//Character.Pawn.Actor.Object //mesh, tmp
extern const DWORD MESH; //[Offset: 0x320, Size: 4]

extern const DWORD ACTORPOSITION;

//Class: SceneComponent.ActorComponent.Object
extern const DWORD RELATIVELOCATION;
extern const DWORD RELATIVELOCATION3;

#endif