// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <cstring>
#include "SamuraiModule.h"



SamuraiPlatform SamuraiModule::GetSamuraiPlatform(const char* unitClass)
{
	if (std::strncmp(unitClass, "CAV", 3) == 0) return SamuraiPlatform_Cav;
	if (std::strncmp(unitClass, "GEN", 3) == 0) return SamuraiPlatform_Gen;
	if (std::strncmp(unitClass, "ASH", 3) == 0) return SamuraiPlatform_Ash;
	if (std::strncmp(unitClass, "SAM", 3) == 0) return SamuraiPlatform_Sam;
	return SamuraiPlatform_Cav;
}



SamuraiWeapon SamuraiModule::GetSamuraiWeapon(const char* unitClass)
{
	if (std::strlen(unitClass) > 4)
	{
		if (std::strcmp(unitClass + 4, "YARI") == 0) return SamuraiWeapon_Yari;
		if (std::strcmp(unitClass + 4, "KATA") == 0) return SamuraiWeapon_Kata;
		if (std::strcmp(unitClass + 4, "NAGI") == 0) return SamuraiWeapon_Nagi;
		if (std::strcmp(unitClass + 4, "BOW") == 0) return SamuraiWeapon_Bow;
		if (std::strcmp(unitClass + 4, "ARQ") == 0) return SamuraiWeapon_Arq;
	}
	return SamuraiWeapon_Nagi;
}


UnitStats SamuraiModule::GetDefaultUnitStats(const char* unitClass)
{
	SamuraiPlatform platform = GetSamuraiPlatform(unitClass);
	SamuraiWeapon weapon = GetSamuraiWeapon(unitClass);
	return GetDefaultUnitStats(platform, weapon);
}


UnitStats SamuraiModule::GetDefaultUnitStats(SamuraiPlatform platform, SamuraiWeapon weapon)
{
	UnitStats result;

	result.platformType = platform == SamuraiPlatform_Cav || platform == SamuraiPlatform_Gen ? PlatformType::Cavalry : PlatformType::Infantry;

	if (platform == SamuraiPlatform_Cav || platform == SamuraiPlatform_Gen)
	{
		result.fighterSize = glm::vec2(1.1f, 2.3f);
		result.spacing = glm::vec2(1.1f, 1.7f);
		result.walkingSpeed = 7;
		result.runningSpeed = 14;
	}
	else
	{
		result.fighterSize = glm::vec2(0.7f, 0.3f);
		result.spacing = glm::vec2(1.1f, 0.9f);
		result.walkingSpeed = 4;
		result.runningSpeed = 8;
	}

	result.readyingDuration = 1.0f;

	switch (weapon)
	{
		case SamuraiWeapon_Bow:
			result.missileType = MissileType::Bow;
			result.minimumRange = 20;
			result.maximumRange = 150;
			result.runningSpeed += 2; // increased speed
			result.strikingDuration = 3.0f;
			break;

		case SamuraiWeapon_Arq:
			result.missileType = MissileType::Arq;
			result.minimumRange = 20;
			result.maximumRange = 110;
			result.walkingSpeed = 5;
			result.runningSpeed = 9;
			result.strikingDuration = 3.0f;
			break;

		case SamuraiWeapon_Yari:
			result.weaponReach = 5.0f;
			result.strikingDuration = 2.0f;
			break;

		case SamuraiWeapon_Nagi:
			result.weaponReach = 2.4f;
			result.strikingDuration = 1.9f;
			break;

		case SamuraiWeapon_Kata:
			result.weaponReach = 1.0f;
			result.strikingDuration = 1.8f;
			break;
	}

	switch (platform)
	{
		case SamuraiPlatform_Ash:
			result.trainingLevel = 0.5f;
			break;
		case SamuraiPlatform_Gen:
			result.trainingLevel = 0.9f;
			break;
		default:
			result.trainingLevel = 0.8f;
			break;
	}

	return result;
}
