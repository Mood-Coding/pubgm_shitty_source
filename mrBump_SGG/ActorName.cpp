#include "pch.h"
#include "ActorName.h"
#include "Colors.h"

// ActorName -> DisplayName
std::unordered_map<std::string, std::string> ActorDisplayName
{
	//Vehicle
	{"BP_VH_Buggy_2_C", "Buggy"},
	{"BP_VH_Buggy_C", "Buggy"},
	{"BP_VH_Buggy_3_C", "Buggy"},
	{"VH_Motorcycle_1_C", "Motorcycle"},
	{"VH_Dacia_C", "Dacia"},
	{"VH_PG117_C", "PG117"},
	{"VH_UAZ01_C", "UAZ"},
	{"AquaRail_1_C", "Aqua Rail"},
	{"Mirado_open_4_C", "Mirado"},
	{"water_Plane", "Water Plane"},
	{"VH_UAZ02_C", "UAZ"},
	{"VH_UAZ03_C", "UAZ"},
	{"VH_Motorcycle_C", "Motorcycle"},
	{"VH_Dacia_2_C", "Dacia"},
	{"VH_Dacia_3_C", "Dacia"},
	{"VH_CoupeRB_1_C", "Coupe RB"},
	{"VH_Scooter_C", "Scooter"},
	{"VH_MotorcycleCart_C", "Motorcycle Cart"},
	{"VH_Dacia_4_C", "Dacia"},
	{"VH_Motorglider_C", "Motorglider"},
	{"Rony_3_C", "Rony"},
	{"VH_Scooter_4_C", "Rony"},
	{"BP_VH_Bigfoot_C", "Bigfoot"},
	{"VH_Snowbile_C", "Snowmobile"},
	{"BP_VH_Tuk_1_C", "Tuk"},
	{"VH_BRDM_C", "BRDM"},
	{"VH_MiniBus_01_C", "Mini Bus"},
	{"VH_Snowbike_C", "Snowbike"},
	{"VH_MotorcycleCart_1_C", "Motorcycle Cart"},
	{"Rony_01_C", "Rony"},
	{"Mirado_close_3_C", "Mirado"},
	{"BP_AirDropPlane_C", "AirDrop Plane"},

	//melee
	{"BP_WEP_Sickle_Pickup_C", "Sickle"},
	{"BP_WEP_Sickle_Pickup_C_Recycled", "Sickle"},
	{"BP_WEP_Pan_Pickup_C", "Pan"},
	{"BP_WEP_Pan_Pickup_C_Recycled", "Pan"},
	{"BP_WEP_Machete_Pickup_C", "Machete"},
	{"BP_WEP_Machete_Pickup_C_Recycled", "Machete"},
	{"BP_WEP_Cowbar_Pickup_C", "Cowbar"},
	{"BP_WEP_Cowbar_Pickup_C_Recycled", "Cowbar"},

	{"BP_WEP_Pan_C", "Pan"},
	{"BP_WEP_Cowbar_C", "Cowbar"},
	{"BP_WEP_Machete_C", "Machete"},
	{"BP_WEP_Sickle_C", "Machete"},

	//awm 2.0
	{"BP_Other_CrossBow_Wrapper_C", "CrossBow"},
	{"BP_Ammo_Bolt_Pickup_C", "Bolt"},
	{"BP_ZDD_Crossbow_Q_Pickup_C", "Quiver"},

	//sr
	{"BP_Sniper_AWM_Wrapper_C", "AWM"},
	{"BP_Sniper_M24_Wrapper_C", "M24"},
	{"BP_Sniper_Kar98k_Wrapper_C", "Kar98k"},
	{"BP_Sniper_Win94_Wrapper_C", "Win94"},
	{"BP_Sniper_Mosin_Wrapper_C", "Mosin Nagant"},

	//dmr
	{"BP_Sniper_QBU_Wrapper_C", "QBU"},
	{"BP_Sniper_SLR_Wrapper_C", "SLR"},
	{"BP_Sniper_SKS_Wrapper_C", "SKS"},
	{"BP_WEP_Mk14_Pickup_C", "Mk14"},
	{"BP_Sniper_Mini14_Wrapper_C", "Mini14"},
	{"BP_Sniper_VSS_Wrapper_C", "VSS"},
	{"BP_Sniper_MK12_Wrapper_C", "Mk12"},

	//ar
	{"BP_Rifle_AUG_Wrapper_C", "AUG"},
	{"BP_Rifle_M762_Wrapper_C", "M762"},
	{"BP_Rifle_SCAR_Wrapper_C", "SCAR"},
	{"BP_Rifle_QBZ_Wrapper_C", "QBZ"},
	{"BP_Rifle_M416_Wrapper_C", "M416"},
	{"BP_Rifle_M16A4_Wrapper_C", "M16A4"},
	{"BP_Rifle_Mk47_Wrapper_C", "Mk47"},
	{"BP_Rifle_G36_Wrapper_C", "G36"},
	{"BP_Other_DP28_Wrapper_C", "DP28"},
	{"BP_Other_M249_Wrapper_C", "M249"},
	{"BP_Rifle_AKM_Wrapper_C", "AKM"},
	{"BP_Rifle_Groza_Wrapper_C", "Groza"},
	{"BP_Rifle_FAMAS_Wrapper_C", "FAMAS"},
	{"BP_Rifle_AN94_Wrapper_C", "AN94"},
	{"BP_Other_MG3_Wrapper_C", "MG3"},

	//shotgun
	{"BP_ShotGun_DP12_Wrapper_C", "DP12"},
	{"BP_ShotGun_S12K_Wrapper_C", "S12K"},
	{"BP_ShotGun_S686_Wrapper_C", "S686"},
	{"BP_ShotGun_S1897_Wrapper_C", "S1897"},
	{"BP_ShotGun_SawedOff_Wrapper_C", "Sawed Off"},
	{"BP_ShotGun_M1014_Wrapper_C", "M1014"},

	//smg
	{"BP_MachineGun_PP19_Wrapper_C", "PP19"},
	{"BP_MachineGun_TommyGun_Wrapper_C", "Tommy Gun"},
	{"BP_MachineGun_MP5K_Wrapper_C", "MP5K"},
	{"BP_MachineGun_UMP9_Wrapper_C", "UMP9"},
	{"BP_MachineGun_Vector_Wrapper_C", "Vector"},
	{"BP_MachineGun_Uzi_Wrapper_C", "Uzi"},
	{"BP_MachineGun_P90_Wrapper_C", "P90"},

	//pistol
	{"BP_Pistol_R1895_Wrapper_C", "R1895"},
	{"BP_Pistol_R1895_Wrapper_C_Recycled", "R1895"},
	{"BP_Pistol_Vz61_Wrapper_C", "Vz61"},
	{"BP_Pistol_Vz61_Wrapper_C_Recycled", "Vz61"},
	{"BP_Pistol_P92_Wrapper_C", "P92"},
	{"BP_Pistol_P92_Wrapper_C_Recycled", "P92"},
	{"BP_Pistol_P18C_Wrapper_C", "P18C"},
	{"BP_Pistol_P18C_Wrapper_C_Recycled", "P18C"},
	{"BP_Pistol_R45_Wrapper_C", "R45"},
	{"BP_Pistol_R45_Wrapper_C_Recycled", "R45"},
	{"BP_Pistol_P1911_Wrapper_C", "P1911"},
	{"BP_Pistol_P1911_Wrapper_C_Recycled", "P1911"},

	//de 1 tap
	{"BP_Pistol_DesertEagle_Wrapper_C", "Desert Eagle"},

	//ammo
	{"BP_Ammo_762mm_Pickup_C", "7.62mm"},
	{"BP_Ammo_45ACP_Pickup_C", ".45ACP"},
	{"BP_Ammo_556mm_Pickup_C", "5.56mm"},
	{"BP_Ammo_9mm_Pickup_C", "9mm"},
	{"BP_Ammo_300Magnum_Pickup_C", "300Magnum"},
	{"BP_Ammo_12Guage_Pickup_C", "12Guage"},

	{ "BP_Ammo_762mm_Pickup_C_Recycled", "7.62mm" },
	{ "BP_Ammo_45ACP_Pickup_C_Recycled", ".45ACP" },
	{ "BP_Ammo_556mm_Pickup_C_Recycled", "5.56mm" },
	{ "BP_Ammo_9mm_Pickup_C_Recycled", "9mm" },
	{ "BP_Ammo_300Magnum_Pickup_C_Recycled", "300Magnum" },
	{ "BP_Ammo_12Guage_Pickup_C_Recycled", "12Guage" },


	//air drop
	{"BP_Pistol_Flaregun_Wrapper_C", "Flaregun"},
	{"BP_Ammo_Flare_Pickup_C", "Flare" },

	//scope
	{"BP_MZJ_8X_Pickup_C", "x8"},
	{"BP_MZJ_6X_Pickup_C", "x6"},
	{"BP_MZJ_4X_Pickup_C", "x4"},
	{"BP_MZJ_3X_Pickup_C", "x3"},
	{"BP_MZJ_3X_Pickup_C_Recycled", "x3"},
	{"BP_MZJ_2X_Pickup_C", "x2"},
	{"BP_MZJ_QX_Pickup_C", "Holo"},
	{"BP_MZJ_QX_Pickup_C_Recycled", "Holo"},
	{"BP_MZJ_HD_Pickup_C", "Reddot"},
	{"BP_MZJ_SideRMR_Pickup_C", "Canted Sight"},
	{"BP_MZJ_2X_Pickup_C_Recucled", "x2"},

	//bag
	{"PickUp_BP_Bag_Lv3_C", "Bag 3"},
	{"PickUp_BP_Bag_Lv3_C_Recycled", "Bag 3"},
	{"PickUp_BP_Bag_Lv3_B_C", "Bag 3"},
	{"PickUp_BP_Bag_Lv2_C", "Bag 2"},
	{"PickUp_BP_Bag_Lv2_C_Recycled", "Bag 2"},
	{"PickUp_BP_Bag_Lv2_B_C", "Bag 2"},
	{"PickUp_BP_Bag_Lv1_C", "Bag 1"},
	{"PickUp_BP_Bag_Lv1_C_Recycled", "Bag 1"},
	{"PickUp_BP_Bag_Lv1_B_C", "Bag 1"},

	//armor
	{"PickUp_BP_Armor_Lv3_C", "Armor 3"},
	{"PickUp_BP_Armor_Lv2_C", "Armor 2"},
	{"PickUp_BP_Armor_Lv1_C", "Armor 1"},
	{ "PickUp_BP_Armor_Lv3_C_Recycled", "Armor 3" },
	{ "PickUp_BP_Armor_Lv2_C_Recycled", "Armor 2" },
	{ "PickUp_BP_Armor_Lv1_C_Recycled", "Armor 1" },

	//helmet
	{"PickUp_BP_Helmet_Lv3_C", "Helmet 3"},
	{"PickUp_BP_Helmet_Lv3_C_Recycled", "Helmet 3"},
	{"PickUp_BP_Helmet_Lv2_C", "Helmet 2"},
	{"PickUp_BP_Helmet_Lv2_C_Recycled", "Helmet 2"},
	{"PickUp_BP_Helmet_Lv2_B_C", "Helmet 2"},
	{"PickUp_BP_Helmet_Lv1_C", "Helmet 1"},
	{"PickUp_BP_Helmet_Lv1_C_Recycled", "Helmet 1"},
	{"PickUp_BP_Helmet_Lv1_B_C", "Helmet 1"},

	//sr mag
	{"BP_DJ_Sniper_E_Pickup_C", "SR-Mag Extended"},
	{"BP_DJ_Sniper_Q_Pickup_C", "SR-Mag Quickdraw"},
	{"BP_DJ_Sniper_EQ_Pickup_C", "SR-Mag Extended Quickdraw"},

	//ar mag
	{"BP_DJ_Large_E_Pickup_C", "AR-Mag Extended"},
	{"BP_DJ_Large_E_Pickup_C_Recycled", "AR-Mag Extended"},
	{"BP_DJ_Large_EQ_Pickup_C", "AR-Mag Extended Quickdraw"},
	{"BP_DJ_Large_Q_Pickup_C", "AR-Mag QuickDraw"},

	//smg mag
	{"BP_DJ_Mid_E_Pickup_C", "SMG-Mag Extended"},
	{"BP_DJ_Mid_EQ_Pickup_C", "SMG-Mag Extended Quickdraw"},
	{"BP_DJ_Mid_Q_Pickup_C", "SMG-Mag QuickDraw"},

	//compensator
	{"BP_QK_Sniper_Compensator_Pickup_C", "SR Compensator"},
	{"BP_QK_Large_Compensator_Pickup_C", "AR Compensator"},
	{"BP_QK_Mid_Compensator_Pickup_C", "SMG Compensator"},

	//flashhider
	{"BP_QK_Sniper_FlashHider_Pickup_C", "SR FlashHider"},
	{"BP_QK_Large_FlashHider_Pickup_C", "AR FlashHider"},
	{"BP_QK_Mid_FlashHider_Pickup_C", "SMG FlashHider"},

	//suppressor
	{"BP_QK_Sniper_Suppressor_Pickup_C", "SR Suppressor"},
	{"BP_QK_Large_Suppressor_Pickup_C", "AR Suppressor"},
	{"BP_QK_Mid_Suppressor_Pickup_C", "SMG Suppressor"},

	{"BP_QK_Large_ExtendedBarrel_Pickup_C", "Extended Barrel"},

	//stock
	{"BP_QT_A_Pickup_C", "Tactical Stock"},
	{"BP_QT_Sniper_Pickup_C", "Check Pad"},
	{"BP_QT_UZI_Pickup_C", "UZI Stock"},
	{ "BP_QT_A_Pickup_C_Recycled", "Tactical Stock" },
	{ "BP_QT_Sniper_Pickup_C_Recycled", "Check Pad" },
	{ "BP_QT_UZI_Pickup_C_Recycled", "UZI Stock" },


	//grip
	{"BP_WB_Angled_Pickup_C", "Angled Foregrip"},
	{"BP_WB_HalfGrip_Pickup_C", "Half Grip"},
	{"BP_WB_Vertical_Pickup_C", "Vertical Foregrip"},
	{"BP_WB_Lasersight_Pickup_C", "Lasersight"},
	{"BP_WB_LightGrip_Pickup_C", "Light Grip"},
	{"BP_WB_ThumbGrip_Pickup_C", "Thumb Grip"},

	//shotgun attachements
	{ "BP_QK_DuckBill_Pickup_C", "Duckbill" },
	{ "BP_QK_Choke_Pickup_C", "Choke" },

	//misc
	{"BP_ZDD_Sniper_Pickup_C", "Bullet Loop" },
	{"GasCan_Destructible_Pickup_C", "Gas Can" },
	{"BP_SpikeTrap_Wrapper_C", "Spike Trap" },

	{ "BP_ZDD_Sniper_Pickup_C_Recycled", "Bullet Loop" },
	{ "GasCan_Destructible_Pickup_C_Recycled", "Gas Can" },
	{ "BP_SpikeTrap_Wrapper_C_Recycled", "Spike Trap" },

	//energy
	{"Firstaid_Pickup_C", "Firstaid"},
	{"Firstaid_Pickup_C_Recycled", "Firstaid"},
	{"FirstAidbox_Pickup_C", "Medkit"},
	{"Injection_Pickup_C", "Adrenaline"},
	{"Pills_Pickup_C", "Pain Killer"},
	{"Pills_Pickup_C_Recycled", "Pain Killer"},
	{"Drink_Pickup_C", "Energy Drink"},
	{"Drink_Pickup_C_Recycled", "Energy Drink"},
	{"Bandage_Pickup_C", "Bandage"},
	{"Bandage_Pickup_C_Recycled", "Bandage"},

	// BP_Weapon_GrenadePackage_Bandage_C
	//boom
	{"BP_Grenade_Shoulei_Weapon_Wrapper_C", "Bomb"},
	{"BP_Grenade_Shoulei_C", "<!> Bomb <!>"},
	{"BP_Grenade_Smoke_Weapon_Wrapper_C", "Smoke"},
	{"BP_Grenade_Burn_Weapon_Wrapper_C", "Molotov" },
	{"BP_Grenade_Stun_Weapon_Wrapper_C", "Stun" },
	{"BP_Grenade_Apple_Weapon_Wrapper_C", "Apple"},

	{"BP_Grenade_Smoke_C", "Smoke" }

		//BP_AirDropPlane_C
};

// ActorName -> Color filter ID
std::unordered_map<std::string, byte> ActorColorFilterID
{
	//air drop
	{"Flare", 8},
	{"Flaregun", 8},


	//melee
	{"Sickle", 1},
	{"Pan", 2},
	{"Machete", 1},
	{"Cowbar", 1},

	//awm 2.0
	{"CrossBow", 1},
	{"Bolt", 1},
	{"Quiver", 1},

	//sr
	{"AWM", 8},
	{"M24", 7},
	{"Kar98k", 7},
	{"Win94", 7},
	{"Mosin Nagant", 7},

	//dmr
	{"QBC", 7},
	{"SLR", 7},
	{"SKS", 7},
	{"Mk17", 7},
	{"Mini14", 7},
	{"VSS", 7},
	{"MK17", 7},

	//ar
	{"AUG", 7},
	{"M762", 7},
	{"SCAR", 7},
	{"DP28", 7},
	{"Mk14", 7},
	{"Mk47", 7},
	{"QBZ", 7},
	{"QBU", 7},
	{"M416", 7},
	{"M16A4", 7},
	{"Mk12", 7},
	{"G36", 7},
	{"DP78", 7},
	{"M249", 7},
	{"AKM", 7},
	{"Groza", 7},
	{"FAMAS", 7},
	{"AN94", 7},
	{"MG3", 7},

	//shotgun
	{"DP12", 6},
	{"S12K", 6},
	{"S686", 6},
	{"S1897", 6},
	{"Sawed Off", 6},
	{"M1014", 6},

	//smg
	{"PP19", 6},
	{"Tommy Gun", 6},
	{"MP5K", 6},
	{"UMP9", 6},
	{"Vector", 6},
	{"Vz61", 6},
	{"Uzi", 6},
	{"P90", 6},

	//pistol
	{"R1895", 4},
	{"Vz71", 4},
	{"P92", 4},
	{"P18C", 4},
	{"R45", 4},
	{"P1911", 4},

	//de 1 tap
	{"Desert Eagle", 4},

	//ammo
	{"7.62mm", 4},
	{".45ACP", 4},
	{"5.56mm", 4},
	{"9mm", 4},
	{"300Magnum", 8},
	{"12Guage", 4 },

	//scope
	{"x8", 8 },
	{"x6", 6 },
	{"x4", 6 },
	{"x3", 5 },
	{"x2", 5 },
	{"Holo", 5 },
	{"Reddot", 5},
	{"Canted Sight", 5 },

	//bag
	{"Bag 3", 8},
	{"Bag 2", 6},
	{"Bag 1", 2},

	//armor
	{"Armor 3", 8},
	{"Armor 2", 6},
	{"Armor 1", 2},

	//helmet
	{"Helmet 3", 8},
	{"Helmet 2", 6},
	{"Helmet 1", 2},

	//sr mag
	{"SR-Mag Extended", 5 },
	{"SR-Mag Quickdraw", 5 },
	{"SR-Mag Extended Quickdraw", 6 },

	//ar mag
	{"AR-Mag Extended", 5 },
	{"AR-Mag Extended Quickdraw", 6 },
	{"AR-Mag QuickDraw",5 },

	//smg mag
	{"SMG-Mag Extended", 5 },
	{"SMG-Mag Extended Quickdraw", 6 },
	{"SMG-Mag QuickDraw", 5 },

	//compensator
	{"SR Compensator", 5 },
	{"AR Compensator", 5 },
	{"SMG Compensator", 5 },

	//flashhider
	{"SR FlashHider", 5 },
	{"AR FlashHider", 5 },
	{"SMG FlashHider", 5 },

	//suppressor
	{"SR Suppressor", 5 },
	{"AR Suppressor", 5 },
	{"SMG Suppressor", 5 },

	{"Extended Barrel", 5 },

	//stock
	{"Tactical Stock", 5 },
	{"Check Pad", 5 },
	{"UZI Stock", 5 },

	//grip
	{"Angled Foregrip", 5 },
	{"Half Grip", 5 },
	{"Vertical Foregrip",5 },
	{"Lasersight", 5 },
	{"Light Grip", 5 },
	{"Thumb Grip",5 },

	//shotgun attachements
	{ "Duckbill", 2 },
	{ "Choke", 2 },

	//misc
	{"Bullet Loop", 2 },
	{"Gas Can", 5 },
	{"Spike Trap", 2 },

	{"Player Crate", 5 },
	{"Air Drop", 8 },
	{"Air Bomb", 8 },

	//energy
	{"Firstaid", 4},
	{"Medkit", 4 },
	{"Adrenaline", 4 },
	{"Pain Killer", 4 },
	{"Energy Drink", 4 },
	{"Bandage", 4 },

	//boom
	{"Bomb", 3 },
	{"<!> Bomb <!>", 8 },
	{"Smoke", 3},
	{"Molotov", 3 },
	{"Stun", 3 },
	{"Apple", 1 }
};

// Color filter ID -> Color
std::unordered_map<int, unsigned int> ColorFilter
{
	{1, DARKERGRAY(255)},
	{2, WHITE(255)},
	{3, GREEN(255)},
	{4, BLUE(255)},
	{5, CYAN(255)},
	{6, ORANGE(255)},
	{7, YELLOW(255)},
	{8, RED(255)},
};

// TypeSpecificID -> Display Name
std::unordered_map<int, std::string> DisplayName
{
	{101008, "M762"},
	{101002, "M16A4"},
	{101003, "SCAR"},
	{101004, "M416"},
	{101010, "G36"},
	{101006, "AUG"},
	{101101, "ASM"},
	{101001, "AKM"},
	{101005, "Groza"},
	{103003, "AWM"},
	{103002, "M24"},
	{103001, "Kar98k"},
	{103011, "Mosin"},
	{106007, "Flaregun"},
	{105001, "M249"},
	{105002, "DP28"},
	{105010, "MG3"},
	{203014, "3X"},
	{203004, "4x"},
	{203015, "6x"},
	{203005, "8x"},
	{502002, "Helmet Lv.2"},
	{502003, "Helmet Lv.3"},
	{503002, "Armour Lv.2"},
	{503003, "Armour Lv.3"},
	{501006, "Bag Lv.3"},
	{501005, "Bag Lv.2"},
};


//else if (code == 101003)
//return "SCAR-L";
//else if (code == 101004)
//return "M416";
//else if (code == 101002)
//return "M16A-4";
//else if (code == 103003)
//return "AWM";
//else if (code == 103010)
//return "QBU";
//else if (code == 103009)
//return "SLR";
//else if (code == 103004)
//return "SKS";
//else if (code == 103006)
//return "Mini14";
//else if (code == 103002)
//return "M24";
//else if (code == 103001)
//return "Kar98k";
//else if (code == 103005)
//return "VSS";
//else if (code == 103008)
//return "Win94";
//else if (code == 101009)
//return "Mk47 Mutant";
//else if (code == 101010)
//return "G36C";
//else if (code == 101007)
//return "QBZ";
//else if (code == 101001)
//return "AKM";
//else if (code == 101005)
//return "Groza";
//else if (code == 101006)
//return "AUG_A3";
//else if (code == 104003)
//return "S12K";
//else if (code == 104004)
//return "DBS";
//else if (code == 104001)
//return "S686";
//else if (code == 104002)
//return "S1897";
//else if (code == 106006)
//return "SawedOff";
//else if (code == 102005)
//return "PP19 Bizon";
//else if (code == 102004)
//return "TommyGun";
//else if (code == 102007)
//return "MP5K";
//else if (code == 102002)
//return "UMP9";
//else if (code == 102003)
//return "Vector";
//else if (code == 102001)
//return "Uzi";
//else if (code == 106003)
//return "R1895";
//else if (code == 106008)
//return "Vz61";
//else if (code == 106001)
//return "P92";
//else if (code == 106004)
//return "P18C";
//else if (code == 106005)
//return "R45";
//else if (code == 106002)
//return "P1911";
//else if (code == 106010)
//return "DesertEagle";
//else if (code == 108003)
//return "Sickle";
//else if (code == 108001)
//return "Machete";
//else if (code == 107001)
//return "Cross Bow";
//else if (code == 108004)
//return "Pan";
//else if (code == 103007)
//return "Mk14";
//else if (code == 302001)
//return "7.62";
//else if (code == 305001)
//return "45ACP";
//else if (code == 303001)
//return "5.56";
//else if (code == 301001)
//return "9mm";
//else if (code == 306001)
//return "300Magnum";
//else if (code == 304001)
//return "12Guage";
//else if (code == 307001)
//return "Arrows";
//else if (code == 201010)
//return "Rifle-FlashHider";
//else if (code == 201009)
//return "Rifle-Compensator";
//else if (code == 201004)
//return "Mid Compensator";
//else if (code == 205002)
//return "Tactical_Rifle";
//else if (code == 201012)
//return "Duckbill";
//else if (code == 201005)
//return "Sniper_FlashHider";
//else if (code == 201006)
//return "Mid_Suppressor";
//else if (code == 205003)
//return "Chekpad Sniper";
//else if (code == 201001)
//return "Choke";
//else if (code == 205001)
//return "UZI_Pickup";
//else if (code == 201003)
//return "Sniper-Compensator";
//else if (code == 201007)
//return "Sniper-Suppressor";
//else if (code == 201011)
//return "Suppressor-AR";
//else if (code == 204009)
//return "Sniper-Extended";
//else if (code == 204004)
//return "Mid_E";
//else if (code == 204005)
//return "Mid_Q";
//else if (code == 204007)
//return "Sniper_Mag";
//else if (code == 204008)
//return "Sniper_QuickMag";
//else if (code == 204012)
//return "Rifle_Mag";
//else if (code == 204013)
//return "Exteded AR";
//else if (code == 204011)
//return "Mag-AR";
//else if (code == 204006)
//return "Mid_EQ";
//else if (code == 205004)
//return "Crossbow";
//else if (code == 204014)
//return "ZDD_Sniper";
//else if (code == 203005)
//return "8x";
//else if (code == 203003)
//return "2x";
//else if (code == 203001)
//return "Lazer";
//else if (code == 203014)
//return "3X";
//else if (code == 203002)
//return "Holo";
//else if (code == 203015)
//return "6x";
//else if (code == 203004)
//return "4x";
//else if (code == 105002)
//return "DP28";
//else if (code == 107001)
//return "CrossBow";
//else if (code == 105001)
//return "M249";
//else if (code == 501006)
//return "Bag Lv.3";
//else if (code == 501004)
//return "Bag Lv.1";
//else if (code == 501005)
//return "Bag Lv.2";
//else if (code == 503002)
//return "Armour Lv.2";
//else if (code == 503001)
//return "Armour Lv.1";
//else if (code == 503003)
//return "Armour Lv.3";
//else if (code == 502002)
//return "Helmet Lv.2";
//else if (code == 502001)
//return "Helmet Lv.1";
//else if (code == 502003)
//return "Helmet Lv.3";
//else if (code == 602004)
//return "Grenade";
//else if (code == 602002)
//return "Smoke";
//else if (code == 602003)
//return "Molotof";
//else if (code == 602005)
//return "Apple";
//else if (code == 601003)
//return "Painkiller";
//else if (code == 601002)
//return "Adrenaline";
//else if (code == 601001)
//return "Energy Drink";
//else if (code == 601005)
//return "FirstaidKit";
//else if (code == 601004)
//return "Bandage";
//else if (code == 202006)
//return "ThumbGrip";
//else if (code == 202007)
//return "Lasersight";
//else if (code == 202001)
//return "Angled";
//else if (code == 202004)
//return "LightGrip";
//else if (code == 106007)
//return "Flaregun";
//else if (code == 3000312)
//return "GameCoin";
//else if (code == 601006)
//return "Medkit";
//else if (code == 308001)
//return "Flaregun";
//else if (code == 501003)
//return "Bag Lv.3";
//else if (code == 501002)
//return "Bag Lv.2";
//else if (code == 501001)
//return "Bag Lv.1";
//else if (code == 201002)
//return "Mid_Compensator";
//else if (code == 502005)
//return "Helmet Lv.2";
//else if (code == 403989)
//return "Arctic-Suit";
//else if (code == 403045)
//return "Woodland-Suit";
//else if (code == 403187)
//return "Desert-Suit";
//else if (code == 403188)
//return "Desert-Suit";