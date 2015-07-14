// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleObjects_v1.h"
#include "BattleSimulator_v1_0_0.h"
#include "geometry.h"



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


bool BattleObjects_v1::Unit::IsOwnedBySimulator() const
{
	return _ownedBySimulator;
}


void BattleObjects_v1::Unit::SetOwnedBySimulator(bool value)
{
	_ownedBySimulator = value;
}


bool BattleObjects_v1::Unit::IsFriendlyCommander(BattleObjects::Commander* battleCommander) const
{
	if (battleCommander == nullptr)
		return false;

	if (commander == battleCommander)
		return true;

	if (battleCommander->GetType() == BattleObjects::CommanderType::None)
		return false;

	if (commander->GetTeam() != battleCommander->GetTeam())
		return false;

	return true;
}


bool BattleObjects_v1::Unit::IsCommandableBy(BattleObjects::Commander* battleCommander) const
{
	if (battleCommander == nullptr)
		return false;

	if (commander == battleCommander)
		return true;

	if (battleCommander->GetType() == BattleObjects::CommanderType::None)
		return false;

	if (commander->IsIncapacitated() && commander->GetTeam() == battleCommander->GetTeam())
		return true;

	return false;
}


bool BattleObjects_v1::Unit::IsInMelee() const
{
	int count = 0;
	for (Fighter* fighter = fighters, * end = fighter + fightersCount; fighter != end; ++fighter)
		if (fighter->state.opponent && ++count >= 3)
			return true;

	return false;
}


int BattleObjects_v1::Unit::GetFighterRank(Fighter* fighter)
{
	Unit* unit = fighter->GetUnit();
	return (fighter - unit->fighters) % unit->formation.numberOfRanks;
}


int BattleObjects_v1::Unit::GetFighterFile(Fighter* fighter)
{
	Unit* unit = fighter->GetUnit();
	return (int)(fighter - unit->fighters) / unit->formation.numberOfRanks;
}


BattleObjects_v1::Fighter* BattleObjects_v1::Unit::GetFighter(BattleObjects_v1::Unit* unit, int rank, int file)
{
	if (0 <= rank && rank < unit->formation.numberOfRanks && file >= 0)
	{
		int index = rank + file * unit->formation.numberOfRanks;
		if (index < unit->fightersCount)
			return unit->fighters + index;
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


static bool IsForwardMotion(const std::vector<glm::vec2>& path, glm::vec2 position)
{
	if (path.size() < 2)
		return true;

	glm::vec2 last = *(path.end() - 1);
	glm::vec2 prev = *(path.end() - 2);
	glm::vec2 next = last + (last - prev);

	return glm::length(position - next) < glm::length(position - prev);
}


void BattleObjects_v1::MovementPath::UpdateMovementPathStart(std::vector<glm::vec2>& path, glm::vec2 startPosition)
{
	const float spacing = 10;

	while (!path.empty() && glm::distance(path.front(), startPosition) < spacing)
		path.erase(path.begin());

	path.insert(path.begin(), startPosition);
}


void BattleObjects_v1::MovementPath::UpdateMovementPath(std::vector<glm::vec2>& path, glm::vec2 startPosition, glm::vec2 endPosition)
{
	const float spacing = 10;

	while (!path.empty() && glm::distance(path.front(), startPosition) < spacing)
		path.erase(path.begin());

	while (!path.empty() && glm::distance(path.back(), endPosition) < spacing)
		path.erase(path.end() - 1);

	while (!IsForwardMotion(path, endPosition))
		path.pop_back();

	path.insert(path.begin(), startPosition);

	int n = 20;
	glm::vec2 p = path.back();
	while (n-- != 0 && glm::length(p - endPosition) > 2 * spacing)
	{
		p += spacing * glm::normalize(endPosition - p);
		path.push_back(p);
	}
	if (glm::length(p - endPosition) > spacing)
	{
		path.push_back(0.5f * (p + endPosition));
	}

	path.push_back(endPosition);
}


float BattleObjects_v1::MovementPath::Length(const std::vector<glm::vec2>& path)
{
	float result = 0;

	for (std::vector<glm::vec2>::const_iterator i = path.begin() + 1; i < path.end(); ++i)
		result += glm::distance(*(i - 1), *i);

	return result;
}
