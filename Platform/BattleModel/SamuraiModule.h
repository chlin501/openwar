// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SamuraiModule_H
#define SamuraiModule_H

#include "BattleSimulator_v1_0_0.h"


enum SamuraiPlatform
{
	SamuraiPlatform_Cav = 0, // Cavalry
	SamuraiPlatform_Gen = 1, // General
	SamuraiPlatform_Ash = 2, // Ashigaru
	SamuraiPlatform_Sam = 3  // Samurai
};


enum SamuraiWeapon
{
	SamuraiWeapon_Yari = 0,
	SamuraiWeapon_Kata = 1,
	SamuraiWeapon_Nagi = 2,
	SamuraiWeapon_Bow = 3,
	SamuraiWeapon_Arq = 4
};


struct SamuraiModule
{
	static SamuraiPlatform GetSamuraiPlatform(const char* unitClass);
	static SamuraiWeapon GetSamuraiWeapon(const char* unitClass);

	static BattleObjects_v1::UnitStats GetDefaultUnitStats(const char* unitClass);
	static BattleObjects_v1::UnitStats GetDefaultUnitStats(SamuraiPlatform platform, SamuraiWeapon weapon);
};


#endif
