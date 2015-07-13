// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Algebra/geometry.h"
#include "BattleSimulator.h"
#include "BattleMap/GroundMap.h"
#include "BattleMap/HeightMap.h"
#include "BattleMap/SmoothGroundMap.h"
#include "BattleMap/TiledGroundMap.h"
#include "BattleMap/BattleMap.h"
#include "BattleScript.h"
#include "SamuraiModule.h"
#include "BattleCommander.h"
#include "BattleObserver.h"
#include <glm/gtc/random.hpp>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <set>
#include <sstream>


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


/***/


BattleSimulator::BattleSimulator(BattleMap* battleMap) :
	_battleMap{battleMap}
{
	_dummyCommander = new BattleCommander(this, "", 1, BattleCommanderType::None);
}


BattleSimulator::~BattleSimulator()
{
	for (BattleCommander* commander : _commanders)
		delete commander;

	delete _script;

	delete _dummyCommander;
	for (Unit* unit : _units)
	{
		delete[] unit->fighters;
		delete unit;
	}
}


int BattleSimulator::CountCavalryInMelee() const
{
	int result = 0;

	for (const Unit* unit : _units)
		if (unit->stats.platformType == PlatformType::Cavalry && unit->IsInMelee())
			++result;

	return result;
}


int BattleSimulator::CountInfantryInMelee() const
{
	int result = 0;

	for (const Unit* unit : _units)
		if (unit->stats.platformType == PlatformType::Infantry && unit->IsInMelee())
			++result;

	return result;
}


void BattleSimulator::AddObserver(BattleObserver* observer)
{
	_observers.insert(observer);

	for (Unit* unit : _units)
	{
		observer->OnAddUnit(unit);
		observer->OnCommand(unit, 0);
	}
}


void BattleSimulator::RemoveObserver(BattleObserver* observer)
{
	_observers.erase(observer);
}


BattleMap* BattleSimulator::GetBattleMap() const
{
	return _battleMap;
}


void BattleSimulator::SetDeploymentZone(int team, glm::vec2 center, float radius)
{
	_deploymentZones[team == 1 ? 0 : 1] = glm::vec3{center, radius};
};


glm::vec2 BattleSimulator::GetDeploymentCenter(int team) const
{
	return _deploymentZones[team == 1 ? 0 : 1].xy();
}


float BattleSimulator::GetDeploymentRadius(int team) const
{
	return _deploymentZones[team == 1 ? 0 : 1].z;
}


bool BattleSimulator::IsDeploymentZone(int team, glm::vec2 position) const
{
	glm::vec2 center = GetDeploymentCenter(team);
	float radius = GetDeploymentRadius(team);
	return glm::distance(position, center) < radius;
}


glm::vec2 BattleSimulator::ConstrainDeploymentZone(int team, glm::vec2 position, float inset) const
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


bool BattleSimulator::HasCompletedDeployment(int team) const
{
	int count = 0;
	for (Unit* unit : _units)
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


void BattleSimulator::Deploy(Unit* unit, glm::vec2 position)
{
	unit->state.center = position;

	unit->command = UnitCommand();
	unit->command.bearing = unit->state.bearing;
	unit->nextCommand = unit->command;

	unit->state.unitMode = UnitMode_Initializing;
	MovementRules::AdvanceTime(unit, 0);
	unit->nextState = NextUnitState(unit);
	for (Fighter* i = unit->fighters, * end = i + unit->fightersCount; i != end; ++i)
		i->nextState = NextFighterState(i);

	unit->state = unit->nextState;
	for (Fighter* i = unit->fighters, * end = i + unit->fightersCount; i != end; ++i)
		i->state = i->nextState;
}


Unit* BattleSimulator::AddUnit(BattleCommander* commander, const char* unitClass, int numberOfFighters, UnitStats stats, glm::vec2 position)
{
	Unit* unit = new Unit();

	float bearing = commander->GetTeamPosition() == 1 ? (float)M_PI_2 : (float)M_PI_2 * 3;

	unit->commander = commander;
	unit->unitClass = unitClass;
	unit->stats = stats;

	unit->fightersCount = numberOfFighters;
	unit->fighters = new Fighter[numberOfFighters];

	unit->deployed = !IsDeploymentZone(commander->GetTeam(), position);
	unit->command.bearing = bearing;
	unit->nextCommand = unit->command;

	unit->state.unitMode = UnitMode_Initializing;
	unit->state.center = position;
	unit->state.waypoint = position;
	unit->state.bearing = bearing;

	unit->formation.rankDistance = stats.fighterSize.y + stats.spacing.y;
	unit->formation.fileDistance = stats.fighterSize.x + stats.spacing.x;
	unit->formation.numberOfRanks = (int)fminf(4, unit->fightersCount);
	unit->formation.numberOfFiles = (int)ceilf((float)unit->fightersCount / unit->formation.numberOfRanks);

	_units.push_back(unit);

	for (Fighter* i = unit->fighters, * end = i + numberOfFighters; i != end; ++i)
		i->unit = unit;

	MovementRules::AdvanceTime(unit, 0);
	unit->nextState = NextUnitState(unit);
	for (Fighter* i = unit->fighters, * end = i + numberOfFighters; i != end; ++i)
		i->nextState = NextFighterState(i);

	unit->state = unit->nextState;
	for (Fighter* i = unit->fighters, * end = i + numberOfFighters; i != end; ++i)
		i->state = i->nextState;

	for (BattleObserver* observer : _observers)
		observer->OnAddUnit(unit);

	return unit;
}


void BattleSimulator::RemoveUnit(Unit* unit)
{
	for (BattleObserver* observer : _observers)
		observer->OnRemoveUnit(unit);

	auto i = std::find(_units.begin(), _units.end(), unit);
	_units.erase(i);

	for (Unit* other : _units)
	{
		for (int i = 0; i < other->fightersCount; ++i)
		{
			Fighter* fighter = &other->fighters[i];
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


void BattleSimulator::NewUnit(int commanderId, const char* unitClass, int strength, glm::vec2 position, float bearing)
{
	UnitStats unitStats = SamuraiModule::GetDefaultUnitStats(unitClass);

	BattleCommander* commander = GetCommanders()[commanderId - 1];

	Unit* unit = AddUnit(commander, unitClass, strength, unitStats, position);
	unit->command.bearing = glm::radians(90 - bearing);
}


void BattleSimulator::SetUnitCommand(Unit* unit, const UnitCommand& command, float timer)
{
	unit->nextCommand = command;
	unit->nextCommandTimer = timer;

	if (unit->nextCommandTimer <= 0)
	{
		unit->command = unit->nextCommand;
		unit->nextCommandTimer = 0;
	}

	for (BattleObserver* observer : _observers)
		observer->OnCommand(unit, timer);
}


void BattleSimulator::AddShooting(const Shooting& shooting, float timer)
{
	_shootings.push_back(std::pair<float, Shooting>(timer, shooting));

	for (BattleObserver* observer : _observers)
		observer->OnShooting(shooting, timer);
}


void BattleSimulator::AdvanceTime(float secondsSinceLastTime)
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
		for (Unit* unit : _units)
		{
			UpdateUnitRange(unit);
		}
	}

	if (_winnerTeam == 0)
	{
		std::map<int, int> total;
		std::map<int, int> routing;

		for (Unit* unit : _units)
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


void BattleSimulator::SimulateOneTimeStep()
{
	for (Unit* unit : _units)
	{
		if (!unit->deployed && !IsDeploymentZone(unit->commander->GetTeam(), unit->state.center))
			unit->deployed = true;
	}

	for (Unit* unit : _units)
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

	for (Unit* unit : _units)
	{
		MovementRules::AdvanceTime(unit, _timeStep);
	}

	ComputeNextState();
	AssignNextState();

	ResolveMeleeCombat();
	ResolveMissileCombat();
	RemoveCasualties();
	RemoveDeadUnits();
	RemoveFinishedShootings();
}


void BattleSimulator::RebuildQuadTree()
{
	_fighterQuadTree.clear();
	_weaponQuadTree.clear();

	for (Unit* unit : _units)
	{
		if (unit->state.unitMode != UnitMode_Initializing)
		{
			for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
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


void BattleSimulator::ComputeNextState()
{
	for (Unit* unit : _units)
	{
		unit->nextState = NextUnitState(unit);

		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
			fighter->nextState = NextFighterState(fighter);
	}
}


void BattleSimulator::AssignNextState()
{
	for (Unit* unit : _units)
	{
		if (!unit->state.IsRouting() && unit->nextState.IsRouting())
			for (BattleObserver* observer : _observers)
				observer->OnRouting(unit);

		unit->state = unit->nextState;

		if (unit->state.IsRouting())
		{
			unit->command.ClearPathAndSetDestination(unit->state.center);
			unit->command.meleeTarget = nullptr;
		}

		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			fighter->state = fighter->nextState;
		}

		UpdateUnitRange(unit);
	}
}


void BattleSimulator::UpdateUnitRange(Unit* unit)
{
	UnitRange& unitRange = unit->unitRange;

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


void BattleSimulator::ResolveMeleeCombat()
{
	for (Unit* unit : _units)
	{
		bool isMissile = unit->stats.missileType != MissileType::None;
		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			Fighter* meleeTarget = fighter->state.meleeTarget;
			if (meleeTarget && meleeTarget->unit->IsOwnedBySimulator())
			{
				Unit* enemyUnit = meleeTarget->unit;
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
					meleeTarget->state.readyState = ReadyState_Stunned;
					meleeTarget->state.stunnedTimer = 0.6f;
				}

				fighter->state.readyingTimer = fighter->unit->stats.readyingDuration;
			}
		}
	}
}


void BattleSimulator::ResolveMissileCombat()
{
	for (Unit* unit : _units)
	{
		if (unit->IsOwnedBySimulator() && unit->state.shootingCounter > unit->shootingCounter)
		{
			TriggerShooting(unit);
			unit->shootingCounter = unit->state.shootingCounter;
		}
	}

	ResolveProjectileCasualties();
}


void BattleSimulator::TriggerShooting(Unit* unit)
{
	if (!unit->deployed)
		return;
	if (unit->state.IsRouting())
		return;
	if (unit->command.missileTarget == nullptr)
		return;

	Shooting shooting;
	shooting.unit = unit;
	shooting.missileType = unit->stats.missileType;
	shooting.target = unit->command.missileTarget->state.center;

	bool arq = shooting.missileType == MissileType::Arq;
	float distance = 0;

	for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
	{
		if (fighter->state.readyState == ReadyState_Prepared)
		{
			float dx = 10.0f * ((rand() & 255) / 128.0f - 1.0f);
			float dy = 10.0f * ((rand() & 255) / 127.0f - 1.0f);

			Projectile projectile;
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


void BattleSimulator::ResolveProjectileCasualties()
{
	static int random = 0;

	for (std::pair<float, Shooting>& s : _shootings)
	{
		if (s.first > 0)
		{
			s.first -= _timeStep;
		}

		if (s.first <= 0)
		{
			Shooting& shooting = s.second;

			if (!shooting.released)
			{
				for (BattleObserver* observer : _observers)
					observer->OnRelease(s.second);
				shooting.released = true;
			}

			shooting.timeToImpact -= _timeStep;

			std::vector<Projectile>::iterator i = shooting.projectiles.begin();
			while (i != shooting.projectiles.end())
			{
				Projectile& projectile = *i;
				if (shooting.timeToImpact + projectile.delay <= 0)
				{
					glm::vec2 hitpoint = projectile.position2;
					for (quadtree<Fighter*>::iterator j(_fighterQuadTree.find(hitpoint.x, hitpoint.y, 0.45f)); *j; ++j)
					{
						Fighter* fighter = **j;
						if (fighter->unit->IsOwnedBySimulator())
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


void BattleSimulator::RemoveCasualties()
{
	bounds2f bounds = _battleMap->GetHeightMap()->GetBounds();
	glm::vec2 center = bounds.mid();
	float radius = bounds.x().size() / 2;
	float radius_squared = radius * radius;

	for (Unit* unit : _units)
	{
		if (unit->IsOwnedBySimulator())
		{
			Fighter* end = unit->fighters + unit->fightersCount;
			for (Fighter* fighter = unit->fighters; fighter != end; ++fighter)
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

	for (Unit* unit : _units)
	{
		Fighter* end = unit->fighters + unit->fightersCount;
		for (Fighter* fighter = unit->fighters; fighter != end; ++fighter)
			if (fighter->state.opponent && fighter->state.opponent->casualty)
				fighter->state.opponent = nullptr;
	}

	for (Unit* unit : _units)
	{
		std::vector<Fighter> fighters;

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

		for (BattleObserver* observer : _observers)
			for (const Fighter& fighter : fighters)
				observer->OnCasualty(fighter);

	}
}


void BattleSimulator::RemoveDeadUnits()
{
	std::vector<Unit*> remove;

	for (Unit* unit : _units)
		if (unit->fightersCount == 0)
			remove.push_back(unit);

	for (Unit* unit : remove)
		RemoveUnit(unit);
}


void BattleSimulator::RemoveFinishedShootings()
{
	auto i = std::remove_if(_shootings.begin(), _shootings.end(), [](const std::pair<float, Shooting>& s) { return s.second.projectiles.empty(); });
	_shootings.erase(i, _shootings.end());
}


UnitState BattleSimulator::NextUnitState(Unit* unit)
{
	UnitState result;

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
		&& !IsWithinLineOfFire(unit, unit->command.missileTarget->state.center))
	{
		unit->command.missileTargetLocked = false;
		unit->command.missileTarget = nullptr;
	}

	if (unit->state.unitMode != UnitMode_Standing || unit->command.missileTarget == nullptr)
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
			&& IsWithinLineOfFire(unit, unit->command.missileTarget->state.center))
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

	for (Unit* other : _units)
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

	result.waypoint = MovementRules::NextWaypoint(unit);

	return result;
}


Unit* BattleSimulator::ClosestEnemyWithinLineOfFire(Unit* unit)
{
	Unit* closestEnemy = 0;
	float closestDistance = 10000;
	for (Unit* target : _units)
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


bool BattleSimulator::IsWithinLineOfFire(Unit* unit, glm::vec2 position)
{
	return unit->unitRange.IsWithinRange(position);
}


UnitMode BattleSimulator::NextUnitMode(Unit* unit)
{
	switch (unit->state.unitMode)
	{
		case UnitMode_Initializing:
			return UnitMode_Standing;

		case UnitMode_Standing:
			if (unit->command.path.size() > 2 || glm::length(unit->state.center - unit->command.GetDestination()) > 8)
				return UnitMode_Moving;
			break;

		case UnitMode_Moving:
			if (unit->command.path.size() <= 2 && glm::length(unit->state.center - unit->command.GetDestination()) <= 8)
				return UnitMode_Standing;
			break;

		default:
			break;
	}
	return unit->state.unitMode;
}


float BattleSimulator::NextUnitDirection(Unit* unit)
{
	if (true) // unit->movement
		return unit->command.bearing;
	else
		return unit->state.bearing;
}


FighterState BattleSimulator::NextFighterState(Fighter* fighter)
{
	const FighterState& original = fighter->state;
	FighterState result;

	result.readyState = original.readyState;
	result.position = NextFighterPosition(fighter);
	result.position_z = _battleMap->GetHeightMap()->InterpolateHeight(result.position);
	result.velocity = NextFighterVelocity(fighter);


	// DIRECTION

	if (fighter->unit->state.unitMode == UnitMode_Moving)
	{
		result.bearing = angle(original.velocity);
	}
	else if (original.opponent)
	{
		result.bearing = angle(original.opponent->state.position - original.position);
	}
	else
	{
		result.bearing = fighter->unit->state.bearing;
	}


	// OPPONENT

	if (original.opponent
		&& (original.opponent - original.opponent->unit->fighters) < original.opponent->unit->fightersCount
		&& glm::length(original.position - original.opponent->state.position) <= fighter->unit->stats.weaponReach * 2)
	{
		result.opponent = original.opponent;
	}
	else if (fighter->unit->state.unitMode != UnitMode_Moving && !fighter->unit->state.IsRouting())
	{
		result.opponent = FindFighterStrikingTarget(fighter);
	}

	// DESTINATION

	result.destination = MovementRules::NextFighterDestination(fighter);

	// READY STATE

	switch (original.readyState)
	{
		case ReadyState_Unready:
			if (fighter->unit->command.meleeTarget)
			{
				result.readyState = ReadyState_Prepared;
			}
			else if (fighter->unit->state.unitMode == UnitMode_Standing)
			{
				result.readyState = ReadyState_Readying;
				result.readyingTimer = fighter->unit->stats.readyingDuration;
			}
			break;

		case ReadyState_Readying:
			if (original.readyingTimer > _timeStep)
			{
				result.readyingTimer = original.readyingTimer - _timeStep;
			}
			else
			{
				result.readyingTimer = 0;
				result.readyState = ReadyState_Prepared;
			}
			break;

		case ReadyState_Prepared:
			if (fighter->unit->state.unitMode == UnitMode_Moving && fighter->unit->command.meleeTarget == nullptr)
			{
				result.readyState = ReadyState_Unready;
			}
			else if (result.opponent)
			{
				result.readyState = ReadyState_Striking;
				result.strikingTimer = fighter->unit->stats.strikingDuration;
			}
			break;

		case ReadyState_Striking:
			if (original.strikingTimer > _timeStep)
			{
				result.strikingTimer = original.strikingTimer - _timeStep;
				result.opponent = original.opponent;
			}
			else
			{
				result.meleeTarget = original.opponent;
				result.strikingTimer = 0;
				result.readyState = ReadyState_Readying;
				result.readyingTimer = fighter->unit->stats.readyingDuration;
			}
			break;

		case ReadyState_Stunned:
			if (original.stunnedTimer > _timeStep)
			{
				result.stunnedTimer = original.stunnedTimer - _timeStep;
			}
			else
			{
				result.stunnedTimer = 0;
				result.readyState = ReadyState_Readying;
				result.readyingTimer = fighter->unit->stats.readyingDuration;
			}
			break;
	}

	return result;
}


glm::vec2 BattleSimulator::NextFighterPosition(Fighter* fighter)
{
	Unit* unit = fighter->unit;

	if (unit->state.unitMode == UnitMode_Initializing)
	{
		glm::vec2 center = unit->state.center;
		glm::vec2 frontLeft = unit->formation.GetFrontLeft(center);
		glm::vec2 offsetRight = unit->formation.towardRight * (float)Unit::GetFighterFile(fighter);
		glm::vec2 offsetBack = unit->formation.towardBack * (float)Unit::GetFighterRank(fighter);
		return frontLeft + offsetRight + offsetBack;
	}
	else
	{
		glm::vec2 result = fighter->state.position + fighter->state.velocity * _timeStep;
		glm::vec2 adjust;
		int count = 0;

		const float fighterDistance = 0.9f;

		for (quadtree<Fighter*>::iterator i(_fighterQuadTree.find(result.x, result.y, fighterDistance)); *i; ++i)
		{
			Fighter* obstacle = **i;
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

		for (quadtree<Fighter*>::iterator i(_weaponQuadTree.find(result.x, result.y, weaponDistance)); *i; ++i)
		{
			Fighter* obstacle = **i;
			if (obstacle->unit->commander->GetTeam() != unit->commander->GetTeam())
			{
				glm::vec2 r = obstacle->unit->stats.weaponReach * vector2_from_angle(obstacle->state.bearing);
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


glm::vec2 BattleSimulator::NextFighterVelocity(Fighter* fighter)
{
	Unit* unit = fighter->unit;
	float speed = unit->GetSpeed();
	glm::vec2 destination = fighter->state.destination;

	switch (fighter->state.readyState)
	{
		case ReadyState_Striking:
			speed = unit->stats.walkingSpeed / 4;
			break;

		case ReadyState_Stunned:
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
		if (unit->stats.platformType == PlatformType::Cavalry)
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


Fighter* BattleSimulator::FindFighterStrikingTarget(Fighter* fighter)
{
	Unit* unit = fighter->unit;

	glm::vec2 position = fighter->state.position + unit->stats.weaponReach * vector2_from_angle(fighter->state.bearing);
	float radius = 1.1f;

	for (quadtree<Fighter*>::iterator i(_fighterQuadTree.find(position.x, position.y, radius)); *i; ++i)
	{
		Fighter* target = **i;
		if (target != fighter && target->unit->commander->GetTeam() != unit->commander->GetTeam())
		{
			return target;
		}
	}

	return nullptr;
}


bool BattleSimulator::TeamHasAbandondedBattle(int team) const
{
	for (Unit* unit : _units)
		if (unit->commander->GetTeam() == team && !unit->commander->HasAbandonedBattle())
			return false;

	return true;
}


/***/


void BattleSimulator::SetScript(BattleScript* value)
{
	delete _script;
	_script = value;
}


void BattleSimulator::SetTeamPosition(int team, int position)
{
	if (team == 1)
		_teamPosition1 = position;
	if (team == 2)
		_teamPosition2 = position;
}


int BattleSimulator::GetTeamPosition(int team) const
{
	if (team == 1)
		return _teamPosition1;
	if (team == 2)
		return _teamPosition2;
	return 0;
}


BattleCommander* BattleSimulator::AddCommander(const char* playerId, int team, BattleCommanderType type)
{
	BattleCommander* commander = new BattleCommander(this, playerId, team, type);
	_commanders.push_back(commander);
	return commander;
}


BattleCommander* BattleSimulator::GetCommander(const char* playerId) const
{
	for (BattleCommander* commander : _commanders)
		if (std::strcmp(commander->GetPlayerId(), playerId) == 0)
			return commander;

	return nullptr;
}


BattleCommander* BattleSimulator::GetDummyCommander() const
{
	return _dummyCommander;
}


void BattleSimulator::Tick(double secondsSinceLastTick)
{
	if (_script)
		_script->Tick(secondsSinceLastTick);

	UpdateDeploymentZones(secondsSinceLastTick);
}


void BattleSimulator::EnableDeploymentZones(float deploymentTimer)
{
	_deploymentTimer = deploymentTimer;
	_deploymentEnabled = true;
	UpdateDeploymentZones(0);
}


void BattleSimulator::UpdateDeploymentZones(double secondsSinceLastTick)
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
