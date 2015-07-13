// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleObjects_v1.h"
#include "BattleSimulator.h"
#include "geometry.h"



BattleCommander::BattleCommander(BattleSimulator* simulator, const char* playerId, int team, BattleCommanderType type) :
	_simulator{simulator},
	_playerId{playerId},
	_team{team},
	_type{type}
{
}


const char* BattleCommander::GetPlayerId() const
{
	return _playerId.c_str();
}


int BattleCommander::GetTeam() const
{
	return _team;
}


BattleCommanderType BattleCommander::GetType() const
{
	return _type;
}


int BattleCommander::GetTeamPosition() const
{
	return _simulator->GetTeamPosition(_team);
}


bool BattleCommander::IsIncapacitated() const
{
	return _isIncapacitated;
}


void BattleCommander::SetIncapacitated(bool value)
{
	_isIncapacitated = value;
}


bool BattleCommander::HasAbandonedBattle() const
{
	return _hasAbandonedBattle;
}


void BattleCommander::SetAbandonedBattle(bool value)
{
	_hasAbandonedBattle = value;
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


bool UnitRange::IsWithinRange(glm::vec2 p) const
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



glm::vec2 Unit::CalculateUnitCenter()
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


float Unit::GetSpeed()
{
	if (state.IsRouting())
		return stats.runningSpeed * 1.2f;
	else
		return command.running || (command.meleeTarget != nullptr) ? stats.runningSpeed : stats.walkingSpeed;
}


bool Unit::IsOwnedBySimulator() const
{
	return _ownedBySimulator;
}


void Unit::SetOwnedBySimulator(bool value)
{
	_ownedBySimulator = value;
}


bool Unit::IsFriendlyCommander(BattleCommander* battleCommander) const
{
	if (battleCommander == nullptr)
		return false;

	if (commander == battleCommander)
		return true;

	if (battleCommander->GetType() == BattleCommanderType::None)
		return false;

	if (commander->GetTeam() != battleCommander->GetTeam())
		return false;

	return true;
}


bool Unit::IsCommandableBy(BattleCommander* battleCommander) const
{
	if (battleCommander == nullptr)
		return false;

	if (commander == battleCommander)
		return true;

	if (battleCommander->GetType() == BattleCommanderType::None)
		return false;

	if (commander->IsIncapacitated() && commander->GetTeam() == battleCommander->GetTeam())
		return true;

	return false;
}


bool Unit::IsInMelee() const
{
	int count = 0;
	for (Fighter* fighter = fighters, * end = fighter + fightersCount; fighter != end; ++fighter)
		if (fighter->state.opponent && ++count >= 3)
			return true;

	return false;
}


int Unit::GetFighterRank(Fighter* fighter)
{
	Unit* unit = fighter->unit;
	return (fighter - unit->fighters) % unit->formation.numberOfRanks;
}


int Unit::GetFighterFile(Fighter* fighter)
{
	Unit* unit = fighter->unit;
	return (int)(fighter - unit->fighters) / unit->formation.numberOfRanks;
}


Fighter* Unit::GetFighter(Unit* unit, int rank, int file)
{
	if (0 <= rank && rank < unit->formation.numberOfRanks && file >= 0)
	{
		int index = rank + file * unit->formation.numberOfRanks;
		if (index < unit->fightersCount)
			return unit->fighters + index;
	}
	return 0;
}


glm::vec2 Formation::GetFrontLeft(glm::vec2 center)
{
	return center - towardRight * (0.5f * (numberOfFiles - 1)) - towardBack * (0.5f * (numberOfRanks - 1));
}


void Formation::SetDirection(float direction)
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


void MovementPath::UpdateMovementPathStart(std::vector<glm::vec2>& path, glm::vec2 startPosition)
{
	const float spacing = 10;

	while (!path.empty() && glm::distance(path.front(), startPosition) < spacing)
		path.erase(path.begin());

	path.insert(path.begin(), startPosition);
}


void MovementPath::UpdateMovementPath(std::vector<glm::vec2>& path, glm::vec2 startPosition, glm::vec2 endPosition)
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


float MovementPath::Length(const std::vector<glm::vec2>& path)
{
	float result = 0;

	for (std::vector<glm::vec2>::const_iterator i = path.begin() + 1; i < path.end(); ++i)
		result += glm::distance(*(i - 1), *i);

	return result;
}
