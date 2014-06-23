// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <algorithm>
#include "Algebra/geometry.h"
#include "BattleSimulator.h"
#include "MovementRules.h"
#include "BattleCommander.h"


Formation::Formation() :
rankDistance(0),
fileDistance(0),
numberOfRanks(0),
numberOfFiles(0),
_direction(0)
{
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


void MovementRules::UpdateMovementPathStart(std::vector<glm::vec2>& path, glm::vec2 startPosition)
{
	const float spacing = 10;

	while (!path.empty() && glm::distance(path.front(), startPosition) < spacing)
		path.erase(path.begin());

	path.insert(path.begin(), startPosition);
}


void MovementRules::UpdateMovementPath(std::vector<glm::vec2>& path, glm::vec2 startPosition, glm::vec2 endPosition)
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


float MovementRules::Length(const std::vector<glm::vec2>& path)
{
	float result = 0;

	for (std::vector<glm::vec2>::const_iterator i = path.begin() + 1; i < path.end(); ++i)
		result += glm::distance(*(i - 1), *i);

	return result;
}


glm::vec2 MovementRules::NextWaypoint(Unit* unit)
{
	for (glm::vec2 p : unit->command.path)
		if (glm::distance(p, unit->state.center) > 1.0f)
			return p;

	if (unit->command.meleeTarget != nullptr)
		return unit->command.meleeTarget->state.center;

	if (!unit->command.path.empty())
		return unit->command.path.back();

	return unit->state.center;
}



void MovementRules::AdvanceTime(Unit* unit, float timeStep)
{
	if (unit->command.meleeTarget != nullptr)
		unit->command.UpdatePath(unit->state.center, unit->command.meleeTarget->state.center);
	else if (unit->command.path.empty())
		unit->command.ClearPathAndSetDestination(unit->state.center);
	else
		unit->command.UpdatePath(unit->state.center, unit->command.path.back());


	float count = unit->fightersCount;
	float ranks = unit->formation.numberOfRanks;

	unit->formation.numberOfRanks = (int)fminf(4, count);
	unit->formation.numberOfFiles = (int)ceilf(count / ranks);

	float direction = unit->command.bearing;

	if (unit->command.path.size() > 1)
	{
		glm::vec2 diff = unit->command.path[1] - unit->command.path[0];
		if (glm::length(diff) > 5)
			direction = ::angle(diff);
	}

	if (unit->command.meleeTarget && glm::length(unit->state.center - unit->command.meleeTarget->state.center) <= 15)
		direction = angle(unit->command.meleeTarget->state.center - unit->state.center);

	if (fabsf(direction - unit->formation._direction) > 0.1f)
	{
		unit->timeUntilSwapFighters = 0;
	}

	unit->formation.SetDirection(direction);

	if (unit->timeUntilSwapFighters <= timeStep)
	{
		SwapFighters(unit);
		unit->timeUntilSwapFighters = 5;
	}
	else
	{
		unit->timeUntilSwapFighters -= timeStep;
	}
}


struct FighterPos
{
	Fighter* fighter;
	FighterState state;
	glm::vec2 pos;
};


static bool SortLeftToRight(const FighterPos& v1, const FighterPos& v2) { return v1.pos.y > v2.pos.y; }
static bool SortFrontToBack(const FighterPos& v1, const FighterPos& v2) { return v1.pos.x > v2.pos.x; }


void MovementRules::SwapFighters(Unit* unit)
{
	std::vector<FighterPos> fighters;

	float direction = unit->formation._direction;

	Fighter* fightersEnd = unit->fighters + unit->fightersCount;

	for (Fighter* fighter = unit->fighters; fighter != fightersEnd; ++fighter)
	{
		FighterPos fighterPos;
		fighterPos.fighter = fighter;
		fighterPos.state = fighter->state;
		fighterPos.pos = rotate(fighter->state.position, -direction);
		fighters.push_back(fighterPos);
	}

	std::sort(fighters.begin(), fighters.end(), SortLeftToRight);

	int index = 0;
	while (index < unit->fightersCount)
	{
		int count = unit->fightersCount - index;
		if (count > unit->formation.numberOfRanks)
			count = unit->formation.numberOfRanks;

		std::vector<FighterPos>::iterator begin = fighters.begin() + index;
		std::sort(begin, begin + count, SortFrontToBack);
		while (count-- != 0)
		{
			unit->fighters[index].state = fighters[index].state;
			++index;
		}
	}
}


glm::vec2 MovementRules::NextFighterDestination(Fighter* fighter)
{
	Unit* unit = fighter->unit;

	const UnitState& unitState = unit->state;
	const FighterState& fighterState = fighter->state;

	if (unitState.IsRouting())
	{
		if (unit->commander->GetTeamPosition() == 1)
			return glm::vec2(fighterState.position.x * 3, -2000);
		else
			return glm::vec2(fighterState.position.x * 3, 2000);
	}

	if (fighterState.opponent != nullptr)
	{
		return fighterState.opponent->state.position
			- unit->stats.weaponReach * vector2_from_angle(fighterState.bearing);
	}

	switch (fighterState.readyState)
	{
		case ReadyState_Striking:
		case ReadyState_Stunned:
			return fighterState.position;
		default:
			break;
	}

	int rank = Unit::GetFighterRank(fighter);
	int file = Unit::GetFighterFile(fighter);
	glm::vec2 destination;
	if (rank == 0)
	{
		if (unitState.unitMode == UnitMode_Moving)
		{
			destination = fighterState.position;
			int n = 1;
			for (int i = 1; i <= 5; ++i)
			{
				Fighter* other = Unit::GetFighter(unit, rank, file - i);
				if (other == 0)
					break;
				destination += other->state.position + (float)i * unit->formation.towardRight;
				++n;
			}
			for (int i = 1; i <= 5; ++i)
			{
				Fighter* other = Unit::GetFighter(unit, rank, file + i);
				if (other == nullptr)
					break;
				destination += other->state.position - (float)i * unit->formation.towardRight;
				++n;
			}
			destination /= n;
			destination -= glm::normalize(unit->formation.towardBack) * unit->GetSpeed();
		}
		else if (unitState.unitMode == UnitMode_Turning)
		{
			glm::vec2 frontLeft = unit->formation.GetFrontLeft(unitState.center);
			destination = frontLeft + unit->formation.towardRight * (float)file;
		}
		else
		{
			glm::vec2 frontLeft = unit->formation.GetFrontLeft(unitState.waypoint);
			destination = frontLeft + unit->formation.towardRight * (float)file;
		}
	}
	else
	{
		Fighter* fighterLeft = Unit::GetFighter(unit, rank - 1, file - 1);
		Fighter* fighterMiddle = Unit::GetFighter(unit, rank - 1, file);
		Fighter* fighterRight = Unit::GetFighter(unit, rank - 1, file + 1);

		if (fighterLeft == nullptr || fighterRight == nullptr)
		{
			destination = fighterMiddle->state.destination;
		}
		else
		{
			destination = (fighterLeft->state.destination + fighterRight->state.destination) / 2.0f;
		}
		destination += unit->formation.towardBack;
	}

	return destination;
}
