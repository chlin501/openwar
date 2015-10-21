// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleObjects_v1.h"
#include "BattleSimulator_v1_0_0.h"
#include "Algebra/geometry.h"

#include <cstring>


int BattleObjects_v1::Fighter::GetRank() const
{
	return static_cast<int>(this - unit->fighters) % unit->formation.numberOfRanks;
}


int BattleObjects_v1::Fighter::GetFile() const
{
	return static_cast<int>(this - unit->fighters) / unit->formation.numberOfRanks;
}



static float normalize_angle(float a)
{
	static float two_pi = 2.0f * (float)M_PI;
	while (a < 0)
		a += two_pi;
	while (a > two_pi)
		a -= two_pi;
	return a;
}


bool BattleObjects::UnitRange::IsWithinRange(glm::vec2 p) const
{
	if (minimumRange > 0 && maximumRange > 0)
	{
		glm::vec2 diff = p - center;
		float angle = glm::atan(diff.y, diff.x);
		float angleDelta = 0.5f * angleLength;

		if (glm::abs(diff_radians(angle, angleStart + angleDelta)) > angleDelta)
			return false;

		float distance = glm::length(diff);
		if (distance < minimumRange)
			return false;

		if (!actualRanges.empty())
		{
			float n = actualRanges.size() - 1;
			float k = n * normalize_angle(angle - angleStart) / angleLength;
			float i = glm::floor(k);

			float a0 = actualRanges[(int)i];
			float a1 = actualRanges[(int)i + 1];
			float actualRange = glm::mix(a0, a1, k - i);

			return distance <= actualRange;
		}
	}

	return false;
}



glm::vec2 BattleObjects_v1::Unit::CalculateUnitCenter()
{
	if (state.unitMode == UnitMode_Initializing)
		return state.center;

	glm::vec2 p = glm::vec2();
	int count = 0;

	for (Fighter* fighter = fighters, * end = fighter + fightersCount; fighter != end; ++fighter)
	{
		p += fighter->state.position;
		++count;
	}

	return p / (float)count;
}


float BattleObjects_v1::Unit::GetSpeed()
{
	if (state.IsRouting())
		return stats.runningSpeed * 1.2f;
	else
		return command.running || (command.meleeTarget != nullptr) ? stats.runningSpeed : stats.walkingSpeed;
}


bool BattleObjects_v1::Unit::IsInMelee() const
{
	int count = 0;
	for (Fighter* fighter = fighters, * end = fighter + fightersCount; fighter != end; ++fighter)
		if (fighter->state.opponent && ++count >= 3)
			return true;

	return false;
}


BattleObjects_v1::Fighter* BattleObjects_v1::Unit::GetFighter(int rank, int file) const
{
	if (0 <= rank && rank < formation.numberOfRanks && file >= 0)
	{
		int index = rank + file * formation.numberOfRanks;
		if (index < fightersCount)
			return fighters + index;
	}
	return 0;
}


glm::vec2 BattleObjects::Formation::GetFrontLeft(glm::vec2 center) const
{
	return center - towardRight * (0.5f * (numberOfFiles - 1)) - towardBack * (0.5f * (numberOfRanks - 1));
}


void BattleObjects::Formation::SetDirection(float direction)
{
	_direction = direction;
	towardRight = glm::vec2((float)sin(direction), -(float)cos(direction)) * fileDistance;
	towardBack = glm::vec2(-(float)cos(direction), -(float)sin(direction)) * rankDistance;
}


void BattleObjects_v1::Unit::SetFighterCasualty(glm::vec2 position)
{
	Fighter* fighter = nullptr;
	float distance = 0.0f;
	for (int i = 0; i < fightersCount; ++i)
		if (!fighters[i].casualty)
		{
			float d = glm::length(fighters[i].state.position - position);
			if (fighter == nullptr || d < distance)
			{
				fighter = &fighters[i];
				distance = d;
			}
		}

	if (fighter)
	{
		fighter->casualty = true;
		timeUntilSwapFighters = 0.2f;
	}
}



BattleObjects_v1::SamuraiPlatform BattleObjects_v1::GetSamuraiPlatform(const char* unitClass)
{
	if (std::strncmp(unitClass, "CAV", 3) == 0) return SamuraiPlatform_Cav;
	if (std::strncmp(unitClass, "GEN", 3) == 0) return SamuraiPlatform_Gen;
	if (std::strncmp(unitClass, "ASH", 3) == 0) return SamuraiPlatform_Ash;
	if (std::strncmp(unitClass, "SAM", 3) == 0) return SamuraiPlatform_Sam;
	return SamuraiPlatform_Cav;
}



BattleObjects_v1::SamuraiWeapon BattleObjects_v1::GetSamuraiWeapon(const char* unitClass)
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


BattleObjects_v1::UnitStats BattleObjects_v1::GetDefaultUnitStats(const char* unitClass)
{
	SamuraiPlatform platform = GetSamuraiPlatform(unitClass);
	SamuraiWeapon weapon = GetSamuraiWeapon(unitClass);
	return GetDefaultUnitStats(platform, weapon);
}


BattleObjects_v1::UnitStats BattleObjects_v1::GetDefaultUnitStats(SamuraiPlatform platform, SamuraiWeapon weapon)
{
	BattleObjects_v1::UnitStats result;

	result.platformType = platform == SamuraiPlatform_Cav || platform == SamuraiPlatform_Gen ? BattleObjects::PlatformType::Cavalry : BattleObjects::PlatformType::Infantry;

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
			result.missileType = BattleObjects::MissileType::Bow;
			result.minimumRange = 20;
			result.maximumRange = 150;
			result.runningSpeed += 2; // increased speed
			result.strikingDuration = 3.0f;
			break;

		case SamuraiWeapon_Arq:
			result.missileType = BattleObjects::MissileType::Arq;
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
