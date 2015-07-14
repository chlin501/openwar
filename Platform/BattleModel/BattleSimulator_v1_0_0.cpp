// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Algebra/geometry.h"
#include "BattleSimulator_v1_0_0.h"
#include "BattleMap/GroundMap.h"
#include "BattleMap/HeightMap.h"
#include "BattleMap/SmoothGroundMap.h"
#include "BattleMap/TiledGroundMap.h"
#include "BattleMap/BattleMap.h"
#include "BattleScript.h"
#include "SamuraiModule.h"
#include "BattleObserver.h"
#include <glm/gtc/random.hpp>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <set>
#include <sstream>


BattleSimulator_v1_0_0::BattleSimulator_v1_0_0(BattleMap* battleMap)
{
	_battleMap = battleMap;
}


BattleSimulator_v1_0_0::~BattleSimulator_v1_0_0()
{
	delete _script;

	for (BattleObjects_v1::Unit* unit : _units)
	{
		delete[] unit->fighters;
		delete unit;
	}
}


int BattleSimulator_v1_0_0::CountCavalryInMelee() const
{
	int result = 0;

	for (const BattleObjects_v1::Unit* unit : _units)
		if (unit->stats.platformType == BattleObjects::PlatformType::Cavalry && unit->IsInMelee())
			++result;

	return result;
}


int BattleSimulator_v1_0_0::CountInfantryInMelee() const
{
	int result = 0;

	for (const BattleObjects_v1::Unit* unit : _units)
		if (unit->stats.platformType == BattleObjects::PlatformType::Infantry && unit->IsInMelee())
			++result;

	return result;
}


void BattleSimulator_v1_0_0::SetDeploymentZone(int team, glm::vec2 center, float radius)
{
	_deploymentZones[team == 1 ? 0 : 1] = glm::vec3{center, radius};
};


glm::vec2 BattleSimulator_v1_0_0::GetDeploymentCenter(int team) const
{
	return _deploymentZones[team == 1 ? 0 : 1].xy();
}


float BattleSimulator_v1_0_0::GetDeploymentRadius(int team) const
{
	return _deploymentZones[team == 1 ? 0 : 1].z;
}


bool BattleSimulator_v1_0_0::IsDeploymentZone(int team, glm::vec2 position) const
{
	glm::vec2 center = GetDeploymentCenter(team);
	float radius = GetDeploymentRadius(team);
	return glm::distance(position, center) < radius;
}


glm::vec2 BattleSimulator_v1_0_0::ConstrainDeploymentZone(int team, glm::vec2 position, float inset) const
{
	float radius = GetDeploymentRadius(team) - inset;
	if (radius > 0)
	{
		glm::vec2 center = GetDeploymentCenter(team);
		glm::vec2 offset = position - center;
		if (glm::length(offset) > radius)
			position = center + radius * glm::normalize(offset);
	}
	return position;
}


bool BattleSimulator_v1_0_0::HasCompletedDeployment(int team) const
{
	int count = 0;
	for (BattleObjects_v1::Unit* unit : _units)
	{
		if (team == unit->commander->GetTeam())
		{
			if (!unit->deployed)
				return false;
			++count;
		}
	}

	return count != 0;
}


void BattleSimulator_v1_0_0::Deploy(BattleObjects::Unit* _unit, glm::vec2 position)
{
	Unit* unit = static_cast<Unit*>(_unit);

	unit->state.center = position;

	unit->command = BattleObjects::UnitCommand();
	unit->command.bearing = unit->state.bearing;
	unit->nextCommand = unit->command;

	unit->state.unitMode = BattleObjects_v1::UnitMode_Initializing;
	MovementRules_AdvanceTime(unit, 0);
	unit->nextState = NextUnitState(unit);
	for (BattleObjects_v1::Fighter* i = unit->fighters, * end = i + unit->fightersCount; i != end; ++i)
		i->nextState = NextFighterState(i);

	unit->state = unit->nextState;
	for (BattleObjects_v1::Fighter* i = unit->fighters, * end = i + unit->fightersCount; i != end; ++i)
		i->state = i->nextState;
}


BattleObjects_v1::Unit* BattleSimulator_v1_0_0::AddUnit(BattleObjects::Commander* commander, const char* unitClass, int numberOfFighters, BattleObjects_v1::UnitStats stats, glm::vec2 position)
{
	BattleObjects_v1::Unit* unit = new BattleObjects_v1::Unit();

	float bearing = commander->GetTeamPosition() == 1 ? (float)M_PI_2 : (float)M_PI_2 * 3;

	unit->commander = commander;
	unit->unitClass = unitClass;
	unit->stats = stats;

	unit->fightersCount = numberOfFighters;
	unit->fighters = new BattleObjects_v1::Fighter[numberOfFighters];

	unit->deployed = !IsDeploymentZone(commander->GetTeam(), position);
	unit->command.bearing = bearing;
	unit->nextCommand = unit->command;

	unit->state.unitMode = BattleObjects_v1::UnitMode_Initializing;
	unit->state.center = position;
	unit->state.waypoint = position;
	unit->state.bearing = bearing;

	unit->formation.rankDistance = stats.fighterSize.y + stats.spacing.y;
	unit->formation.fileDistance = stats.fighterSize.x + stats.spacing.x;
	unit->formation.numberOfRanks = (int)fminf(4, unit->fightersCount);
	unit->formation.numberOfFiles = (int)ceilf((float)unit->fightersCount / unit->formation.numberOfRanks);

	_units_base.push_back(unit);
	_units.push_back(unit);

	for (BattleObjects_v1::Fighter* i = unit->fighters, * end = i + numberOfFighters; i != end; ++i)
		i->unit = unit;

	MovementRules_AdvanceTime(unit, 0);
	unit->nextState = NextUnitState(unit);
	for (BattleObjects_v1::Fighter* i = unit->fighters, * end = i + numberOfFighters; i != end; ++i)
		i->nextState = NextFighterState(i);

	unit->state = unit->nextState;
	for (BattleObjects_v1::Fighter* i = unit->fighters, * end = i + numberOfFighters; i != end; ++i)
		i->state = i->nextState;

	NotifyAddUnit(unit);

	return unit;
}


void BattleSimulator_v1_0_0::RemoveUnit(BattleObjects::Unit* _unit)
{
	Unit* unit = static_cast<Unit*>(_unit);

	NotifyRemoveUnit(unit);

	_units_base.erase(std::find(_units_base.begin(), _units_base.end(), unit));
	_units.erase(std::find(_units.begin(), _units.end(), unit));

	for (BattleObjects_v1::Unit* other : _units)
	{
		for (int i = 0; i < other->fightersCount; ++i)
		{
			BattleObjects_v1::Fighter* fighter = &other->fighters[i];
			if (fighter->state.opponent && fighter->state.opponent->unit == unit)
				fighter->state.opponent = nullptr;
			if (fighter->state.meleeTarget && fighter->state.meleeTarget->unit == unit)
				fighter->state.meleeTarget = nullptr;
		}

		if (other->command.meleeTarget == unit)
			other->command.meleeTarget = nullptr;
		if (other->command.missileTarget == unit)
			other->command.missileTarget = nullptr;

		if (other->nextCommand.meleeTarget == unit)
			other->nextCommand.meleeTarget = nullptr;
		if (other->nextCommand.missileTarget == unit)
			other->nextCommand.missileTarget = nullptr;
	}

	delete[] unit->fighters;
	delete unit;
}


void BattleSimulator_v1_0_0::NewUnit(int commanderId, const char* unitClass, int strength, glm::vec2 position, float bearing)
{
	BattleObjects_v1::UnitStats unitStats = SamuraiModule::GetDefaultUnitStats(unitClass);

	BattleObjects::Commander* commander = GetCommanders()[commanderId - 1];

	BattleObjects_v1::Unit* unit = AddUnit(commander, unitClass, strength, unitStats, position);
	unit->command.bearing = glm::radians(90 - bearing);
}


void BattleSimulator_v1_0_0::SetUnitCommand(BattleObjects::Unit* _unit, const BattleObjects::UnitCommand& command, float timer)
{
	Unit* unit = static_cast<Unit*>(_unit);

	unit->nextCommand = command;
	unit->nextCommandTimer = timer;

	if (unit->nextCommandTimer <= 0)
	{
		unit->command = unit->nextCommand;
		unit->nextCommandTimer = 0;
	}

	NotifyCommand(unit, timer);
}


void BattleSimulator_v1_0_0::IssueUnitCommand(BattleObjects::Unit* unit, const BattleObjects::UnitCommand& command, float timer)
{
	static_cast<BattleObjects_v1::Unit*>(unit)->state.loadingTimer = 0;
	static_cast<BattleObjects_v1::Unit*>(unit)->timeUntilSwapFighters = 0.2f;

	SetUnitCommand(unit, command, timer);
}


void BattleSimulator_v1_0_0::AddShooting(const BattleObjects::Shooting& shooting, float timer)
{
	_shootings.push_back(std::pair<float, BattleObjects::Shooting>(timer, shooting));

	NotifyShooting(shooting, timer);
}


void BattleSimulator_v1_0_0::AdvanceTime(float secondsSinceLastTime)
{
	//if (this)
	//	return;

	bool didStep = false;

	_secondsSinceLastTimeStep += secondsSinceLastTime;
	while (_secondsSinceLastTimeStep >= _timeStep)
	{
		SimulateOneTimeStep();
		_secondsSinceLastTimeStep -= _timeStep;
		didStep = true;
	}

	if (!didStep)
	{
		for (BattleObjects_v1::Unit* unit : _units)
		{
			UpdateUnitRange(unit);
		}
	}

	if (_winnerTeam == 0)
	{
		std::map<int, int> total;
		std::map<int, int> routing;

		for (BattleObjects_v1::Unit* unit : _units)
		{
			int team = unit->commander->GetTeam();
			total[team] += 1;
			if (unit->state.IsRouting())
				routing[team] += 1;
		}

		for (std::pair<int, int> i : total)
		{
			int team = i.first;
			if (routing[team] == i.second)
			{
				_winnerTeam = 3 - team;
				break;
			}
		}

		if (_practice && _winnerTeam == 1)
			_winnerTeam = 0;
	}
}


void BattleSimulator_v1_0_0::SimulateOneTimeStep()
{
	for (BattleObjects_v1::Unit* unit : _units)
	{
		if (!unit->deployed && !IsDeploymentZone(unit->commander->GetTeam(), unit->state.center))
			unit->deployed = true;
	}

	for (BattleObjects_v1::Unit* unit : _units)
	{
		if (unit->nextCommandTimer > 0)
		{
			unit->nextCommandTimer -= _timeStep;
			if (unit->nextCommandTimer <= 0)
			{
				unit->command = unit->nextCommand;
				unit->nextCommandTimer = 0;
			}
		}
	}

	RebuildQuadTree();

	for (BattleObjects_v1::Unit* unit : _units)
	{
		MovementRules_AdvanceTime(unit, _timeStep);
	}

	ComputeNextState();
	AssignNextState();

	ResolveMeleeCombat();
	ResolveMissileCombat();
	RemoveCasualties();
	RemoveDeadUnits();
	RemoveFinishedShootings();
}


void BattleSimulator_v1_0_0::RebuildQuadTree()
{
	_fighterQuadTree.clear();
	_weaponQuadTree.clear();

	for (BattleObjects_v1::Unit* unit : _units)
	{
		if (unit->state.unitMode != BattleObjects_v1::UnitMode_Initializing)
		{
			for (BattleObjects_v1::Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
			{
				_fighterQuadTree.insert(fighter->state.position.x, fighter->state.position.y, fighter);

				if (unit->stats.weaponReach > 0)
				{
					glm::vec2 d = unit->stats.weaponReach * vector2_from_angle(fighter->state.bearing);
					glm::vec2 p = fighter->state.position + d;
					_weaponQuadTree.insert(p.x, p.y, fighter);
				}
			}
		}
	}
}


void BattleSimulator_v1_0_0::ComputeNextState()
{
	for (BattleObjects_v1::Unit* unit : _units)
	{
		unit->nextState = NextUnitState(unit);

		for (BattleObjects_v1::Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
			fighter->nextState = NextFighterState(fighter);
	}
}


void BattleSimulator_v1_0_0::AssignNextState()
{
	for (BattleObjects_v1::Unit* unit : _units)
	{
		if (!unit->state.IsRouting() && unit->nextState.IsRouting())
			NotifyRouting(unit);

		unit->state = unit->nextState;

		if (unit->state.IsRouting())
		{
			unit->command.ClearPathAndSetDestination(unit->state.center);
			unit->command.meleeTarget = nullptr;
		}

		for (BattleObjects_v1::Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			fighter->state = fighter->nextState;
		}

		UpdateUnitRange(unit);
	}
}


void BattleSimulator_v1_0_0::UpdateUnitRange(BattleObjects_v1::Unit* unit)
{
	BattleObjects::UnitRange& unitRange = unit->unitRange;

	unitRange.center = unit->state.center;
	unitRange.angleLength = (float)M_PI_2;
	unitRange.angleStart = unit->state.bearing - 0.5f * unitRange.angleLength;

	unitRange.minimumRange = unit->stats.minimumRange;
	unitRange.maximumRange = unit->stats.maximumRange;

	unitRange.actualRanges.clear();

	if (unitRange.minimumRange > 0 && unitRange.maximumRange > 0)
	{
		float centerHeight = _battleMap->GetHeightMap()->InterpolateHeight(unitRange.center) + 1.9f;

		int n = 24;
		for (int i = 0; i <= n; ++i)
		{
			float angle = unitRange.angleStart + i * unitRange.angleLength / n;
			glm::vec2 direction = vector2_from_angle(angle);
			float delta = (unitRange.maximumRange - unitRange.minimumRange) / 16;
			float maxRange = 0;
			float maxAngle = -100;
			for (float range = unitRange.minimumRange + delta; range <= unitRange.maximumRange; range += delta)
			{
				float height = _battleMap->GetHeightMap()->InterpolateHeight(unitRange.center + range * direction) + 0.5f;
				float verticalAngle = glm::atan(height - centerHeight, range);
				if (verticalAngle > maxAngle)
				{
					maxAngle = verticalAngle;
					maxRange = range;
				}
			}

			unitRange.actualRanges.push_back(glm::max(maxRange, unitRange.minimumRange));
		}
	}
}


void BattleSimulator_v1_0_0::ResolveMeleeCombat()
{
	for (BattleObjects_v1::Unit* unit : _units)
	{
		bool isMissile = unit->stats.missileType != BattleObjects::MissileType::None;
		for (BattleObjects_v1::Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			BattleObjects_v1::Fighter* meleeTarget = fighter->state.meleeTarget;
			if (meleeTarget && meleeTarget->GetUnit()->IsOwnedBySimulator())
			{
				BattleObjects_v1::Unit* enemyUnit = meleeTarget->GetUnit();
				float killProbability = 0.5f;

				killProbability *= 1.25f + unit->stats.trainingLevel;
				killProbability *= 1.25f - enemyUnit->stats.trainingLevel;

				if (isMissile)
					killProbability *= 0.15;

				float heightDiff = fighter->state.position_z - meleeTarget->state.position_z;
				killProbability *= 1.0f + 0.4f * bounds1d(-1.5f, 1.5f).clamp(heightDiff);

				float speed = glm::length(fighter->state.velocity);
				killProbability *= (0.9f + speed / 10.0f);

				float roll = (rand() & 0x7FFF) / (float)0x7FFF;

				if (roll < killProbability)
				{
					meleeTarget->casualty = true;
				}
				else
				{
					meleeTarget->state.readyState = BattleObjects_v1::ReadyState_Stunned;
					meleeTarget->state.stunnedTimer = 0.6f;
				}

				fighter->state.readyingTimer = fighter->GetUnit()->stats.readyingDuration;
			}
		}
	}
}


void BattleSimulator_v1_0_0::ResolveMissileCombat()
{
	for (BattleObjects_v1::Unit* unit : _units)
	{
		if (unit->IsOwnedBySimulator() && unit->state.shootingCounter > unit->shootingCounter)
		{
			TriggerShooting(unit);
			unit->shootingCounter = unit->state.shootingCounter;
		}
	}

	ResolveProjectileCasualties();
}


void BattleSimulator_v1_0_0::TriggerShooting(BattleObjects_v1::Unit* unit)
{
	if (!unit->deployed)
		return;
	if (unit->state.IsRouting())
		return;
	if (unit->command.missileTarget == nullptr)
		return;

	BattleObjects::Shooting shooting;
	shooting.unit = unit;
	shooting.missileType = unit->stats.missileType;
	shooting.target = unit->command.missileTarget->GetCenter();

	bool arq = shooting.missileType == BattleObjects::MissileType::Arq;
	float distance = 0;

	for (BattleObjects_v1::Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
	{
		if (fighter->state.readyState == BattleObjects_v1::ReadyState_Prepared)
		{
			float dx = 10.0f * ((rand() & 255) / 128.0f - 1.0f);
			float dy = 10.0f * ((rand() & 255) / 127.0f - 1.0f);

			BattleObjects::Projectile projectile;
			projectile.position1 = fighter->state.position;
			projectile.position2 = shooting.target + glm::vec2(dx, dy);
			projectile.delay = (arq ? 0.5f : 0.2f) * ((rand() & 0x7FFF) / (float)0x7FFF);
			shooting.projectiles.push_back(projectile);
			distance += glm::length(projectile.position1 - projectile.position2) / unit->fightersCount;
		}
	}

	float speed = arq ? 750 : 75; // meters per second
	shooting.timeToImpact = distance / speed;

	AddShooting(shooting, GetTimerDelay());
}


void BattleSimulator_v1_0_0::ResolveProjectileCasualties()
{
	static int random = 0;

	for (std::pair<float, BattleObjects::Shooting>& s : _shootings)
	{
		if (s.first > 0)
		{
			s.first -= _timeStep;
		}

		if (s.first <= 0)
		{
			BattleObjects::Shooting& shooting = s.second;

			if (!shooting.released)
			{
				NotifyRelease(s.second);
				shooting.released = true;
			}

			shooting.timeToImpact -= _timeStep;

			std::vector<BattleObjects::Projectile>::iterator i = shooting.projectiles.begin();
			while (i != shooting.projectiles.end())
			{
				BattleObjects::Projectile& projectile = *i;
				if (shooting.timeToImpact + projectile.delay <= 0)
				{
					glm::vec2 hitpoint = projectile.position2;
					for (quadtree<BattleObjects_v1::Fighter*>::iterator j(_fighterQuadTree.find(hitpoint.x, hitpoint.y, 0.45f)); *j; ++j)
					{
						BattleObjects_v1::Fighter* fighter = **j;
						if (fighter->GetUnit()->IsOwnedBySimulator())
						{
							bool blocked = false;
							if (fighter->terrainForest)
								blocked = (random++ & 7) <= 5;
							if (!blocked)
								fighter->casualty = true;
						}
					}
					shooting.projectiles.erase(i);
				}
				else
				{
					++i;
				}
				++random;
			}
		}
	}
}


void BattleSimulator_v1_0_0::RemoveCasualties()
{
	bounds2f bounds = _battleMap->GetHeightMap()->GetBounds();
	glm::vec2 center = bounds.mid();
	float radius = bounds.x().size() / 2;
	float radius_squared = radius * radius;

	for (BattleObjects_v1::Unit* unit : _units)
	{
		if (unit->IsOwnedBySimulator())
		{
			BattleObjects_v1::Fighter* end = unit->fighters + unit->fightersCount;
			for (BattleObjects_v1::Fighter* fighter = unit->fighters; fighter != end; ++fighter)
			{
				if (!fighter->casualty)
				{
					fighter->casualty = fighter->terrainImpassable && unit->state.IsRouting();
				}

				if (!fighter->casualty)
				{
					glm::vec2 diff = fighter->state.position - center;
					fighter->casualty = glm::dot(diff, diff) >= radius_squared;
				}
			}
		}
	}

	for (BattleObjects_v1::Unit* unit : _units)
	{
		BattleObjects_v1::Fighter* end = unit->fighters + unit->fightersCount;
		for (BattleObjects_v1::Fighter* fighter = unit->fighters; fighter != end; ++fighter)
			if (fighter->state.opponent && fighter->state.opponent->casualty)
				fighter->state.opponent = nullptr;
	}

	for (BattleObjects_v1::Unit* unit : _units)
	{
		std::vector<BattleObjects_v1::Fighter> fighters;

		int index = 0;
		int n = unit->fightersCount;
		for (int j = 0; j < n; ++j)
		{
			if (unit->fighters[j].casualty)
			{
				++unit->state.recentCasualties;
				fighters.push_back(unit->fighters[j]);
			}
			else
			{
				if (index < j)
					unit->fighters[index].state = unit->fighters[j].state;
				unit->fighters[index].casualty = false;
				index++;
			}
		}

		unit->fightersCount = index;

		_kills[unit->commander->GetTeam()] += fighters.size();

		for (const BattleObjects_v1::Fighter& fighter : fighters)
			NotifyCasualty(unit, fighter.state.position);
	}
}


void BattleSimulator_v1_0_0::RemoveDeadUnits()
{
	std::vector<BattleObjects_v1::Unit*> remove;

	for (BattleObjects_v1::Unit* unit : _units)
		if (unit->fightersCount == 0)
			remove.push_back(unit);

	for (BattleObjects_v1::Unit* unit : remove)
		RemoveUnit(unit);
}


void BattleSimulator_v1_0_0::RemoveFinishedShootings()
{
	auto i = std::remove_if(_shootings.begin(), _shootings.end(), [](const std::pair<float, BattleObjects::Shooting>& s) { return s.second.projectiles.empty(); });
	_shootings.erase(i, _shootings.end());
}


BattleObjects_v1::UnitState BattleSimulator_v1_0_0::NextUnitState(BattleObjects_v1::Unit* unit)
{
	BattleObjects_v1::UnitState result;

	result.center = unit->CalculateUnitCenter();
	result.bearing = NextUnitDirection(unit);
	result.unitMode = NextUnitMode(unit);

	result.shootingCounter = unit->state.shootingCounter;

	if (!unit->command.missileTargetLocked)
	{
		unit->command.missileTarget = ClosestEnemyWithinLineOfFire(unit);
	}
	else if (unit->command.missileTarget
		&& unit->command.missileTarget != unit
		&& !IsWithinLineOfFire(unit, unit->command.missileTarget->GetCenter()))
	{
		unit->command.missileTargetLocked = false;
		unit->command.missileTarget = nullptr;
	}

	if (unit->state.unitMode != BattleObjects_v1::UnitMode_Standing || unit->command.missileTarget == nullptr)
	{
		result.loadingTimer = 0;
		result.loadingDuration = 0;
	}
	else if (unit->state.loadingTimer + _timeStep < unit->state.loadingDuration)
	{
		result.loadingTimer = unit->state.loadingTimer + _timeStep;
		result.loadingDuration = unit->state.loadingDuration;
	}
	else
	{
		if (unit->state.loadingDuration != 0
			&& unit->command.missileTarget
			&& IsWithinLineOfFire(unit, unit->command.missileTarget->GetCenter()))
		{
			++result.shootingCounter;
		}

		result.loadingTimer = 0;
		result.loadingDuration = 4 + (rand() % 100) / 200.0f;
	}

	result.morale = unit->state.morale;
	if (unit->state.recentCasualties > 0)
	{
		result.morale -= unit->state.recentCasualties * (2.4f - unit->stats.trainingLevel) / 100;
	}
	else if (-0.2f < result.morale && result.morale < 1)
	{
		result.morale += (0.1f + unit->stats.trainingLevel) / 2000;
	}

	if (result.morale > -1 && TeamHasAbandondedBattle(unit->commander->GetTeam()))
	{
		result.morale -= 1.0f / 250;
	}

	for (BattleObjects_v1::Unit* other : _units)
	{
		float distance = glm::length(other->state.center - unit->state.center);
		float weight = 1.0f * 50.0f / (distance + 50.0f);
		if (other->commander->GetTeam() == unit->commander->GetTeam())
		{
			result.influence -= weight
					* (1 - other->state.morale)
					* (1 - unit->stats.trainingLevel)
					* other->stats.trainingLevel;
		}
	}

	if (_practice && unit->state.IsRouting() && unit->commander->GetTeam() == 2)
	{
		result.morale = -1;
	}

	if (_winnerTeam != 0 && unit->commander->GetTeam() != _winnerTeam)
	{
		result.morale = -1;
	}

	if (unit->fightersCount <= 8)
	{
		result.morale = -1;
	}

	result.waypoint = MovementRules_NextWaypoint(unit);

	return result;
}


BattleObjects_v1::Unit* BattleSimulator_v1_0_0::ClosestEnemyWithinLineOfFire(BattleObjects_v1::Unit* unit)
{
	BattleObjects_v1::Unit* closestEnemy = 0;
	float closestDistance = 10000;
	for (BattleObjects_v1::Unit* target : _units)
	{
		if (target->commander->GetTeam() != unit->commander->GetTeam() && IsWithinLineOfFire(unit, target->state.center))
		{
			float distance = glm::length(target->state.center - unit->state.center);
			if (distance < closestDistance)
			{
				closestEnemy = target;
				closestDistance = distance;
			}
		}
	}
	return closestEnemy;
}


bool BattleSimulator_v1_0_0::IsWithinLineOfFire(BattleObjects_v1::Unit* unit, glm::vec2 position)
{
	return unit->unitRange.IsWithinRange(position);
}


BattleObjects_v1::UnitMode BattleSimulator_v1_0_0::NextUnitMode(BattleObjects_v1::Unit* unit)
{
	switch (unit->state.unitMode)
	{
		case BattleObjects_v1::UnitMode_Initializing:
			return BattleObjects_v1::UnitMode_Standing;

		case BattleObjects_v1::UnitMode_Standing:
			if (unit->command.path.size() > 2 || glm::length(unit->state.center - unit->command.GetDestination()) > 8)
				return BattleObjects_v1::UnitMode_Moving;
			break;

		case BattleObjects_v1::UnitMode_Moving:
			if (unit->command.path.size() <= 2 && glm::length(unit->state.center - unit->command.GetDestination()) <= 8)
				return BattleObjects_v1::UnitMode_Standing;
			break;

		default:
			break;
	}
	return unit->state.unitMode;
}


float BattleSimulator_v1_0_0::NextUnitDirection(BattleObjects_v1::Unit* unit)
{
	if (true) // unit->movement
		return unit->command.bearing;
	else
		return unit->state.bearing;
}


BattleObjects_v1::FighterState BattleSimulator_v1_0_0::NextFighterState(BattleObjects_v1::Fighter* fighter)
{
	const BattleObjects_v1::FighterState& original = fighter->state;
	BattleObjects_v1::FighterState result;

	result.readyState = original.readyState;
	result.position = NextFighterPosition(fighter);
	result.position_z = _battleMap->GetHeightMap()->InterpolateHeight(result.position);
	result.velocity = NextFighterVelocity(fighter);


	// DIRECTION

	if (fighter->GetUnit()->state.unitMode == BattleObjects_v1::UnitMode_Moving)
	{
		result.bearing = angle(original.velocity);
	}
	else if (original.opponent)
	{
		result.bearing = angle(original.opponent->state.position - original.position);
	}
	else
	{
		result.bearing = fighter->GetUnit()->state.bearing;
	}


	// OPPONENT

	if (original.opponent
		&& (original.opponent - original.opponent->GetUnit()->fighters) < original.opponent->GetUnit()->fightersCount
		&& glm::length(original.position - original.opponent->state.position) <= fighter->GetUnit()->stats.weaponReach * 2)
	{
		result.opponent = original.opponent;
	}
	else if (fighter->GetUnit()->state.unitMode != BattleObjects_v1::UnitMode_Moving && !fighter->GetUnit()->state.IsRouting())
	{
		result.opponent = FindFighterStrikingTarget(fighter);
	}

	// DESTINATION

	result.destination = MovementRules_NextFighterDestination(fighter);

	// READY STATE

	switch (original.readyState)
	{
		case BattleObjects_v1::ReadyState_Unready:
			if (fighter->GetUnit()->command.meleeTarget)
			{
				result.readyState = BattleObjects_v1::ReadyState_Prepared;
			}
			else if (fighter->GetUnit()->state.unitMode == BattleObjects_v1::UnitMode_Standing)
			{
				result.readyState = BattleObjects_v1::ReadyState_Readying;
				result.readyingTimer = fighter->GetUnit()->stats.readyingDuration;
			}
			break;

		case BattleObjects_v1::ReadyState_Readying:
			if (original.readyingTimer > _timeStep)
			{
				result.readyingTimer = original.readyingTimer - _timeStep;
			}
			else
			{
				result.readyingTimer = 0;
				result.readyState = BattleObjects_v1::ReadyState_Prepared;
			}
			break;

		case BattleObjects_v1::ReadyState_Prepared:
			if (fighter->GetUnit()->state.unitMode == BattleObjects_v1::UnitMode_Moving && fighter->GetUnit()->command.meleeTarget == nullptr)
			{
				result.readyState = BattleObjects_v1::ReadyState_Unready;
			}
			else if (result.opponent)
			{
				result.readyState = BattleObjects_v1::ReadyState_Striking;
				result.strikingTimer = fighter->GetUnit()->stats.strikingDuration;
			}
			break;

		case BattleObjects_v1::ReadyState_Striking:
			if (original.strikingTimer > _timeStep)
			{
				result.strikingTimer = original.strikingTimer - _timeStep;
				result.opponent = original.opponent;
			}
			else
			{
				result.meleeTarget = original.opponent;
				result.strikingTimer = 0;
				result.readyState = BattleObjects_v1::ReadyState_Readying;
				result.readyingTimer = fighter->GetUnit()->stats.readyingDuration;
			}
			break;

		case BattleObjects_v1::ReadyState_Stunned:
			if (original.stunnedTimer > _timeStep)
			{
				result.stunnedTimer = original.stunnedTimer - _timeStep;
			}
			else
			{
				result.stunnedTimer = 0;
				result.readyState = BattleObjects_v1::ReadyState_Readying;
				result.readyingTimer = fighter->GetUnit()->stats.readyingDuration;
			}
			break;
	}

	return result;
}


glm::vec2 BattleSimulator_v1_0_0::NextFighterPosition(BattleObjects_v1::Fighter* fighter)
{
	BattleObjects_v1::Unit* unit = fighter->GetUnit();

	if (unit->state.unitMode == BattleObjects_v1::UnitMode_Initializing)
	{
		FighterAssignment assignment = unit->GetFighterAssignment(static_cast<int>(fighter - unit->fighters));
		glm::vec2 center = unit->state.center;
		glm::vec2 frontLeft = unit->formation.GetFrontLeft(center);
		glm::vec2 offsetRight = unit->formation.towardRight * (float)assignment.file;
		glm::vec2 offsetBack = unit->formation.towardBack * (float)assignment.rank;
		return frontLeft + offsetRight + offsetBack;
	}
	else
	{
		glm::vec2 result = fighter->state.position + fighter->state.velocity * _timeStep;
		glm::vec2 adjust;
		int count = 0;

		const float fighterDistance = 0.9f;

		for (quadtree<BattleObjects_v1::Fighter*>::iterator i(_fighterQuadTree.find(result.x, result.y, fighterDistance)); *i; ++i)
		{
			BattleObjects_v1::Fighter* obstacle = **i;
			if (obstacle != fighter)
			{
				glm::vec2 position = obstacle->state.position;
				glm::vec2 diff = position - result;
				float distance2 = glm::dot(diff, diff);
				if (0.01f < distance2 && distance2 < fighterDistance * fighterDistance)
				{
					adjust -= glm::normalize(diff) * fighterDistance;
					++count;
				}
			}
		}

		const float weaponDistance = 0.75f;

		for (quadtree<BattleObjects_v1::Fighter*>::iterator i(_weaponQuadTree.find(result.x, result.y, weaponDistance)); *i; ++i)
		{
			BattleObjects_v1::Fighter* obstacle = **i;
			if (obstacle->GetUnit()->commander->GetTeam() != unit->commander->GetTeam())
			{
				glm::vec2 r = obstacle->GetUnit()->stats.weaponReach * vector2_from_angle(obstacle->state.bearing);
				glm::vec2 position = obstacle->state.position + r;
				glm::vec2 diff = position - result;
				if (glm::dot(diff, diff) < weaponDistance * weaponDistance)
				{
					diff = obstacle->state.position - result;
					adjust -= glm::normalize(diff) * weaponDistance;
					++count;
				}
			}
		}

		if (count != 0)
		{
			result += adjust / (float)count;
		}

		return result;
	}
}


glm::vec2 BattleSimulator_v1_0_0::NextFighterVelocity(BattleObjects_v1::Fighter* fighter)
{
	BattleObjects_v1::Unit* unit = fighter->GetUnit();
	float speed = unit->GetSpeed();
	glm::vec2 destination = fighter->state.destination;

	switch (fighter->state.readyState)
	{
		case BattleObjects_v1::ReadyState_Striking:
			speed = unit->stats.walkingSpeed / 4;
			break;

		case BattleObjects_v1::ReadyState_Stunned:
			speed = unit->stats.walkingSpeed / 4;
			break;

		default:
			break;
	}

	if (_battleMap && glm::length(fighter->state.position - fighter->terrainPosition) > 4)
	{
		fighter->terrainForest = _battleMap->GetGroundMap()->IsForest(fighter->state.position);
		fighter->terrainImpassable = _battleMap->GetGroundMap()->IsImpassable(fighter->state.position);
		if (!fighter->terrainImpassable)
			fighter->terrainPosition = fighter->state.position;
	}

	if (fighter->terrainForest)
	{
		if (unit->stats.platformType == BattleObjects::PlatformType::Cavalry)
			speed *= 0.5;
		else
			speed *= 0.9;
	}

	if (fighter->terrainImpassable)
		destination = fighter->terrainPosition;

	glm::vec2 diff = destination - fighter->state.position;
	float diff_len = glm::dot(diff, diff);
	if (diff_len < 0.3f)
		return diff;

	glm::vec2 delta = glm::normalize(diff) * speed;
	float delta_len = glm::dot(delta, delta);

	return delta_len < diff_len ? delta : diff;
}


BattleObjects_v1::Fighter* BattleSimulator_v1_0_0::FindFighterStrikingTarget(BattleObjects_v1::Fighter* fighter)
{
	BattleObjects_v1::Unit* unit = fighter->GetUnit();

	glm::vec2 position = fighter->state.position + unit->stats.weaponReach * vector2_from_angle(fighter->state.bearing);
	float radius = 1.1f;

	for (quadtree<BattleObjects_v1::Fighter*>::iterator i(_fighterQuadTree.find(position.x, position.y, radius)); *i; ++i)
	{
		BattleObjects_v1::Fighter* target = **i;
		if (target != fighter && target->GetUnit()->commander->GetTeam() != unit->commander->GetTeam())
		{
			return target;
		}
	}

	return nullptr;
}


/***/


void BattleSimulator_v1_0_0::SetScript(BattleScript* value)
{
	delete _script;
	_script = value;
}


void BattleSimulator_v1_0_0::Tick(double secondsSinceLastTick)
{
	if (_script)
		_script->Tick(secondsSinceLastTick);

	UpdateDeploymentZones(secondsSinceLastTick);
}


void BattleSimulator_v1_0_0::EnableDeploymentZones(float deploymentTimer)
{
	_deploymentTimer = deploymentTimer;
	_deploymentEnabled = true;
	UpdateDeploymentZones(0);
}


void BattleSimulator_v1_0_0::UpdateDeploymentZones(double secondsSinceLastTick)
{
	if (_deploymentEnabled)
	{
		const float deploymentRadius = 1024.0f;
		const float deploymenyStart = 128.0f;
		const float deploymentPause = 15.0f; // seconds
		const float deploymentDuration = 45.0f; // seconds (total duration including pause)

		float deploymentOffset = deploymenyStart;
		if (_deploymentTimer > deploymentPause)
			deploymentOffset += (_deploymentTimer - deploymentPause) * (512.0f - deploymenyStart) / (deploymentDuration - deploymentPause);

		for (int team = 1; team <= 2; ++team)
		{
			if (deploymentOffset < 512.0f && !HasCompletedDeployment(team))
			{
				float sign = GetTeamPosition(team) == 1 ? -1.0f : 1.0f;
				SetDeploymentZone(team, glm::vec2{512.0f, 512.0f + sign * (deploymentRadius + deploymentOffset)}, deploymentRadius);
			}
			else
			{
				SetDeploymentZone(team, glm::vec2{}, 0);
			}
		}

		_deploymentTimer += static_cast<float>(secondsSinceLastTick);
	}
	else
	{
		SetDeploymentZone(1, glm::vec2{}, 0);
		SetDeploymentZone(2, glm::vec2{}, 0);
	}
}


glm::vec2 BattleSimulator_v1_0_0::MovementRules_NextWaypoint(BattleObjects_v1::Unit* unit)
{
	for (glm::vec2 p : unit->command.path)
		if (glm::distance(p, unit->state.center) > 1.0f)
			return p;

	if (unit->command.meleeTarget)
		return unit->command.meleeTarget->GetCenter();

	if (!unit->command.path.empty())
		return unit->command.path.back();

	return unit->state.center;
}



void BattleSimulator_v1_0_0::MovementRules_AdvanceTime(BattleObjects_v1::Unit* unit, float timeStep)
{
	if (unit->command.meleeTarget)
		unit->command.UpdatePath(unit->state.center, unit->command.meleeTarget->GetCenter());
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

	if (unit->command.meleeTarget && glm::length(unit->state.center - unit->command.meleeTarget->GetCenter()) <= 15)
		direction = angle(unit->command.meleeTarget->GetCenter() - unit->state.center);

	if (fabsf(direction - unit->formation._direction) > 0.1f)
	{
		unit->timeUntilSwapFighters = 0;
	}

	unit->formation.SetDirection(direction);

	if (unit->timeUntilSwapFighters <= timeStep)
	{
		MovementRules_SwapFighters(unit);
		unit->timeUntilSwapFighters = 5;
	}
	else
	{
		unit->timeUntilSwapFighters -= timeStep;
	}
}


struct FighterPos
{
	BattleObjects_v1::Fighter* fighter;
	BattleObjects_v1::FighterState state;
	glm::vec2 pos;
};


static bool SortLeftToRight(const FighterPos& v1, const FighterPos& v2) { return v1.pos.y > v2.pos.y; }
static bool SortFrontToBack(const FighterPos& v1, const FighterPos& v2) { return v1.pos.x > v2.pos.x; }


void BattleSimulator_v1_0_0::MovementRules_SwapFighters(BattleObjects_v1::Unit* unit)
{
	std::vector<FighterPos> fighters;

	float direction = unit->formation._direction;

	BattleObjects_v1::Fighter* fightersEnd = unit->fighters + unit->fightersCount;

	for (BattleObjects_v1::Fighter* fighter = unit->fighters; fighter != fightersEnd; ++fighter)
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


glm::vec2 BattleSimulator_v1_0_0::MovementRules_NextFighterDestination(BattleObjects_v1::Fighter* fighter)
{
	BattleObjects_v1::Unit* unit = fighter->GetUnit();

	const BattleObjects_v1::UnitState& unitState = unit->state;
	const BattleObjects_v1::FighterState& fighterState = fighter->state;

	if (unitState.IsRouting())
	{
		if (unit->commander->GetTeamPosition() == 1)
			return glm::vec2(fighterState.position.x * 3, -2000);
		else
			return glm::vec2(fighterState.position.x * 3, 2000);
	}

	if (fighterState.opponent)
	{
		return fighterState.opponent->state.position
			- unit->stats.weaponReach * vector2_from_angle(fighterState.bearing);
	}

	switch (fighterState.readyState)
	{
		case BattleObjects_v1::ReadyState_Striking:
		case BattleObjects_v1::ReadyState_Stunned:
			return fighterState.position;
		default:
			break;
	}

	int rank = fighter->GetRank();
	int file = fighter->GetFile();
	glm::vec2 destination;
	if (rank == 0)
	{
		if (unitState.unitMode == BattleObjects_v1::UnitMode_Moving)
		{
			destination = fighterState.position;
			int n = 1;
			for (int i = 1; i <= 5; ++i)
			{
				BattleObjects_v1::Fighter* other = unit->GetFighter(rank, file - i);
				if (other == 0)
					break;
				destination += other->state.position + (float)i * unit->formation.towardRight;
				++n;
			}
			for (int i = 1; i <= 5; ++i)
			{
				BattleObjects_v1::Fighter* other = unit->GetFighter(rank, file + i);
				if (other == nullptr)
					break;
				destination += other->state.position - (float)i * unit->formation.towardRight;
				++n;
			}
			destination /= n;
			destination -= glm::normalize(unit->formation.towardBack) * unit->GetSpeed();
		}
		else if (unitState.unitMode == BattleObjects_v1::UnitMode_Turning)
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
		BattleObjects_v1::Fighter* fighterLeft = unit->GetFighter(rank - 1, file - 1);
		BattleObjects_v1::Fighter* fighterMiddle = unit->GetFighter(rank - 1, file);
		BattleObjects_v1::Fighter* fighterRight = unit->GetFighter(rank - 1, file + 1);

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
