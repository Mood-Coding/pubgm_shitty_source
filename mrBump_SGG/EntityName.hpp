#pragma once
#include "stdafx.h"

#ifndef ENTITYNAME_HPP
#define ENTITYNAME_HPP
std::map<std::string, std::string> EntityName;

bool EntityNameInit() {
    //player
    EntityName["BP_PlayerPawn_SI_C"] = "Player";

    //shit
    EntityName["Useless"] = "Useless";

    //awm version 2
    EntityName["BP_Other_CrossBow_Wrapper_C"] = "CrossBow";

    //sr
    EntityName["BP_Sniper_AWM_Wrapper_C"] = "AWM";
    EntityName["BP_Sniper_M24_Wrapper_C"] = "M24";
    EntityName["BP_Sniper_Kar98k_Wrapper_C"] = "Kar98k";
    EntityName["BP_Sniper_Win94_Wrapper_C"] = "Win94";

    //dmr
    EntityName["BP_Sniper_QBU_Wrapper_C"] = "QBU";
    EntityName["BP_Sniper_SLR_Wrapper_C"] = "SLR";
    EntityName["BP_Sniper_SKS_Wrapper_C"] = "SKS";
    EntityName["BP_WEP_Mk14_Pickup_C"] = "MK14";
    EntityName["BP_Sniper_Mini14_Wrapper_C"] = "Mini14";
    EntityName["BP_Sniper_VSS_Wrapper_C"] = "VSS";

    //ar
    EntityName["BP_Rifle_AUG_C"] = "AUG";
    EntityName["BP_Rifle_M762_Wrapper_C"] = "M762";
    EntityName["BP_Rifle_SCAR_Wrapper_C"] = "SCAR";
    EntityName["BP_Rifle_QBZ_Wrapper_C"] = "QBZ";
    EntityName["BP_Rifle_M416_Wrapper_C"] = "M416";
    EntityName["BP_Rifle_M16A4_Wrapper_C"] = "M16A4";
    EntityName["BP_Rifle_Mk47_Wrapper_C"] = "Mk47";
    EntityName["BP_Rifle_G36_Wrapper_C"] = "G36";
    EntityName["BP_Other_DP28_Wrapper_C"] = "DP28";
    EntityName["BP_Other_M249_Wrapper_C"] = "M249";
    EntityName["BP_Rifle_AKM_Wrapper_C"] = "AKM";
    EntityName["BP_Rifle_Groza_Wrapper_C"] = "Groza";

    //shotgun
    EntityName["BP_ShotGun_DP12_Wrapper_C"] = "DPS";
    EntityName["BP_ShotGun_S12K_Wrapper_C"] = "S12K";
    EntityName["BP_ShotGun_S686_Wrapper_C"] = "S686";
    EntityName["BP_ShotGun_S1897_Wrapper_C"] = "S1897";
    EntityName["BP_ShotGun_SawedOff_Wrapper_C"] = "SawedOff";

    //smg
    EntityName["BP_MachineGun_PP19_Wrapper_C"] = "Bizon";
    EntityName["BP_MachineGun_TommyGun_Wrapper_C"] = "TommyGun";
    EntityName["BP_MachineGun_MP5K_Wrapper_C"] = "MP5K";
    EntityName["BP_MachineGun_UMP9_Wrapper_C"] = "UMP9";
    EntityName["BP_MachineGun_Vector_Wrapper_C"] = "Vector";
    EntityName["BP_MachineGun_Uzi_Wrapper_C"] = "Uzi";

    //pistol
    EntityName["BP_Pistol_R1895_Wrapper_C"] = "R1895";
    EntityName["BP_Pistol_Vz61_Wrapper_C"] = "Vz61";
    EntityName["BP_Pistol_P92_Wrapper_C"] = "P92";
    EntityName["BP_Pistol_P18C_Wrapper_C"] = "P18C";
    EntityName["BP_Pistol_R45_Wrapper_C"] = "R45";
    EntityName["BP_Pistol_P1911_Wrapper_C"] = "P1911";

    //de 1 tap
    EntityName["BP_Pistol_DesertEagle_Wrapper_C"] = "DesertEagle";

    //ammo
    EntityName["BP_Ammo_Flare_Pickup_C"] = "Flare";
    EntityName["BP_Ammo_762mm_Pickup_C"] = "7.62mm";
    EntityName["BP_Ammo_45ACP_Pickup_C"] = ".45ACP";
    EntityName["BP_Ammo_556mm_Pickup_C"] = "5.56mm";
    EntityName["BP_Ammo_9mm_Pickup_C"] = "9mm";
    EntityName["BP_Ammo_300Magnum_Pickup_C"] = "300Magnum";
    EntityName["BP_Ammo_12Guage_Pickup_C"] = "12Guage";
    EntityName["BP_Pistol_Flaregun_Wrapper_C"] = "FlareGun";

    //scope
    EntityName["BP_MZJ_8X_Pickup_C"] = "x8";
    EntityName["BP_MZJ_6X_Pickup_C"] = "x6";
    EntityName["BP_MZJ_4X_Pickup_C"] = "x4";
    EntityName["BP_MZJ_3X_Pickup_C"] = "x3";
    EntityName["BP_MZJ_2X_Pickup_C"] = "x2";
    EntityName["BP_MZJ_QX_Pickup_C"] = "Holo";
    EntityName["BP_MZJ_HD_Pickup_C"] = "Reddot";

    //bag
    EntityName["PickUp_BP_Bag_Lv3_C"] = "Bag Lv3";
    EntityName["PickUp_BP_Bag_Lv2_C"] = "Bag Lv2";
    EntityName["PickUp_BP_Bag_Lv1_C"] = "Bag Lv1";

    //armor
    EntityName["PickUp_BP_Armor_Lv3_C"] = "Armor Lv3";
    EntityName["PickUp_BP_Armor_Lv2_C"] = "Armor Lv2";
    EntityName["PickUp_BP_Armor_Lv1_C"] = "Armor Lv1";

    //helmet
    EntityName["PickUp_BP_Helmet_Lv3_C"] = "Helmet Lv3";
    EntityName["PickUp_BP_Helmet_Lv2_C"] = "Helmet Lv2";
    EntityName["PickUp_BP_Helmet_Lv1_C"] = "Helmet Lv1";

    //sr mag
    EntityName["BP_DJ_Sniper_E_Pickup_C"] = "[SR-Mag] Extended";
    EntityName["BP_DJ_Sniper_Q_Pickup_C"] = "[SR-Mag] Quickdraw";
    EntityName["BP_DJ_Sniper_EQ_Pickup_C"] = "[SR-Mag] Extended Quickdraw";

    //ar mag
    EntityName["BP_DJ_Large_E_Pickup_C"] = "[AR-Mag] Extended";
    EntityName["BP_DJ_Large_EQ_Pickup_C"] = "[AR-Mag] Extended Quickdraw";
    EntityName["BP_DJ_Large_Q_Pickup_C"] = "[AR-Mag] QuickDraw";

    //smg mag
    EntityName["BP_DJ_Mid_E_Pickup_C"] = "[SMG-Mag] Extended";
    EntityName["BP_DJ_Mid_EQ_Pickup_C"] = "[SMG-Mag] Extended Quickdraw";
    EntityName["BP_DJ_Mid_Q_Pickup_C"] = "[SMG-Mag] QuickDraw";

    //compensator
    EntityName["BP_QK_Sniper_Compensator_Pickup_C"] = "[SR] Compensator";
    EntityName["BP_QK_Large_Compensator_Pickup_C"] = "[AR] Compensator";
    EntityName["BP_QK_Mid_Compensator_Pickup_C"] = "[SMG] Compensator";

    //flashhider
    EntityName["BP_QK_Sniper_FlashHider_Pickup_C"] = "[SR] FlashHider";
    EntityName["BP_QK_Large_FlashHider_Pickup_C"] = "[AR] FlashHider";
    EntityName["BP_QK_Mid_FlashHider_Pickup_C"] = "[SMG] FlashHider";

    //suppressor
    EntityName["BP_QK_Sniper_Suppressor_Pickup_C"] = "[SR] Suppressor";
    EntityName["BP_QK_Large_Suppressor_Pickup_C"] = "[AR] ASuppressorR";
    EntityName["BP_QK_Mid_Suppressor_Pickup_C"] = "[SMG] Suppressor";

    //stock
    EntityName["BP_QT_A_Pickup_C"] = "[M416] Tactical Stock";
    EntityName["BP_QT_Sniper_Pickup_C"] = "[SR] Check Pad";
    EntityName["BP_QT_UZI_Pickup_C"] = "[UZI] Stock";

    //grip
    EntityName["BP_WB_Angled_Pickup_C"] = "[Grip] Angled Foregrip";
    EntityName["BP_WB_HalfGrip_Pickup_C"] = "[Grip] HalfGrip";
    EntityName["BP_WB_Vertical_Pickup_C"] = "[Grip] Vertical Foregrip";
    EntityName["BP_WB_Lasersight_Pickup_C"] = "[Grip] Lasersight";
    EntityName["BP_WB_LightGrip_Pickup_C"] = "[Grip] LightGrip";
    EntityName["BP_WB_ThumbGrip_Pickup_C"] = "[Grip] ThumbGrip";

    //energy
    EntityName["Firstaid_Pickup_C"] = "Firstaid";
    EntityName["Injection_Pickup_C"] = "Adrenaline";
    EntityName["Pills_Pickup_C"] = "PainKiller";
    EntityName["Drink_Pickup_C"] = "EnergyDrink";
    EntityName["Bandage_Pickup_C"] = "Bandage";

    //boom
    EntityName["BP_Grenade_Shoulei_Weapon_Wrapper_C"] = "[Nade] Bomb";
    EntityName["BP_Grenade_Smoke_Weapon_Wrapper_C"] = "[Nade] Smoke";
    EntityName["BP_Grenade_Burn_Weapon_Wrapper_C"] = "[Nade] Molotov";

    return 1;
}
#endif