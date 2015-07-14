// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleObjects_v1.h"
#include "BattleSimulator_v1_0_0.h"
#include "geometry.h"


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


bool BattleObjects_v1::UnitRange::IsWithinRange(glm::vec2 p) const
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


glm::vec2 BattleObjects_v1::Formation::GetFrontLeft(glm::vec2 center)
{
	return center - towardRight * (0.5f * (numberOfFiles - 1)) - towardBack * (0.5f * (numberOfRanks - 1));
}


void BattleObjects_v1::Formation::SetDirection(float direction)
{
	_direction = direction;
	towardRight = glm::vec2((float)sin(direction), -(float)cos(direction)) * fileDistance;
	towardBack = glm::vec2(-(float)cos(direction), -(float)sin(direction)) * rankDistance;
}


void BattleObjects_v1::Unit::SetFighterCasualty(glm::vec2 position)
{
	Fighter* fighter = nullptr;
	float distance;
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
