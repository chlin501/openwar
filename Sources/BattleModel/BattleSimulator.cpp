// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <cstring>
#include <set>
#include "../Library/Algebra/geometry.h"
#include "BattleSimulator.h"
#include "GroundMap.h"
#include "HeightMap.h"




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


BattleObserver::~BattleObserver()
{
}


/***/


BattleSimulator::BattleSimulator() :
_fighterQuadTree(0, 0, 1024, 1024),
_weaponQuadTree(0, 0, 1024, 1024),
_secondsSinceLastTimeStep(0),
practice(false),
recentShootings(),
groundMap(nullptr),
heightMap(nullptr),
currentPlayer(0),
lastUnitId(0),
blueTeam(1),
winnerTeam(0),
time(0),
timeStep(1.0f / 15.0f)
{
}


BattleSimulator::~BattleSimulator()
{
	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
		delete[] unit->fighters;
		delete unit;
	}
}



bool BattleSimulator::IsMelee() const
{
	for (std::map<int, Unit*>::const_iterator i = units.begin(); i != units.end(); ++i)
	{
		const Unit* unit = (*i).second;
		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			if (fighter->state.opponent != nullptr)
			{
				return true;
			}
		}
	}
	return false;
}

void BattleSimulator::AddObserver(BattleObserver* observer)
{
	_observers.insert(observer);

	for (std::pair<int, Unit*> i : units)
		observer->OnNewUnit(i.second);
}


void BattleSimulator::RemoveObserver(BattleObserver* observer)
{
	_observers.erase(observer);
}


const std::set<BattleObserver*>&BattleSimulator::GetObservers() const
{
	return _observers;
}


Unit* BattleSimulator::AddUnit(int player, int team, const char* unitClass, int numberOfFighters, UnitStats stats, glm::vec2 position)
{
	Unit* unit = new Unit();

	unit->unitId = ++lastUnitId;
	unit->player = player;
	unit->team = team;
	unit->unitClass = unitClass;
	unit->stats = stats;

	unit->fightersCount = numberOfFighters;
	unit->fighters = new Fighter[numberOfFighters];

	for (Fighter* i = unit->fighters, * end = i + numberOfFighters; i != end; ++i)
		i->unit = unit;

	unit->command.facing = team == 1 ? (float)M_PI_2 : (float)M_PI_2 * 3;

	unit->state.unitMode = UnitMode_Initializing;
	unit->state.center = position;
	unit->state.direction = unit->command.facing;

	unit->command.missileTarget = nullptr;

	unit->formation.rankDistance = stats.fighterSize.y + stats.spacing.y;
	unit->formation.fileDistance = stats.fighterSize.x + stats.spacing.x;
	unit->formation.numberOfRanks = (int)fminf(6, unit->fightersCount);
	unit->formation.numberOfFiles = (int)ceilf((float)unit->fightersCount / unit->formation.numberOfRanks);

	units[unit->unitId] = unit;

	for (BattleObserver* observer : _observers)
		observer->OnNewUnit(unit);

	return unit;
}


void BattleSimulator::DeleteUnit(Unit* unit)
{
	units.erase(unit->unitId);

	for (std::pair<int, Unit*> i : units)
	{
		if (i.second->command.meleeTarget == unit)
			i.second->command.meleeTarget = nullptr;
		if (i.second->command.missileTarget == unit)
			i.second->command.missileTarget = nullptr;
	}

	delete[] unit->fighters;
	delete unit;
}


void BattleSimulator::AdvanceTime(float secondsSinceLastTime)
{
	//if (this != nullptr)
	//	return;

	recentShootings.clear();

	bool didStep = false;

	_secondsSinceLastTimeStep += secondsSinceLastTime;
	while (_secondsSinceLastTimeStep >= timeStep)
	{
		SimulateOneTimeStep();
		_secondsSinceLastTimeStep -= timeStep;
		didStep = true;
	}

	if (!didStep)
	{
		for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
		{
			Unit* unit = (*i).second;
			UpdateUnitRange(unit);
		}
	}

	for (BattleObserver* observer : _observers)
	{
		for (const Shooting& shooting : recentShootings)
			observer->OnShooting(shooting);
	}

	if (winnerTeam == 0)
	{
		int count1 = 0;
		int count2 = 0;

		for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
		{
			Unit* unit = (*i).second;
			if (!unit->state.IsRouting())
			{
				switch (unit->team)
				{
					case 1:
						++count1;
						break;
					case 2:
						++count2;
						break;
					default:
						break;
				}
			}
		}

		if (count1 == 0)
			winnerTeam = 2;
		else if (count2 == 0)
			winnerTeam = 1;
	}
}


void BattleSimulator::SimulateOneTimeStep()
{
	RebuildQuadTree();

	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
		MovementRules::AdvanceTime(unit, timeStep);
	}

	ComputeNextState();
	AssignNextState();

	ResolveMeleeCombat();
	ResolveMissileCombat();
	RemoveCasualties();
	RemoveDeadUnits();

	time += timeStep;
}


void BattleSimulator::RebuildQuadTree()
{
	_fighterQuadTree.clear();
	_weaponQuadTree.clear();

	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
		if (unit->state.unitMode != UnitMode_Initializing)
		{
			for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
			{
				_fighterQuadTree.insert(fighter->state.position.x, fighter->state.position.y, fighter);

				if (unit->stats.weaponReach > 0)
				{
					glm::vec2 d = unit->stats.weaponReach * vector2_from_angle(fighter->state.direction);
					glm::vec2 p = fighter->state.position + d;
					_weaponQuadTree.insert(p.x, p.y, fighter);
				}
			}
		}
	}
}


void BattleSimulator::ComputeNextState()
{
	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
		unit->nextState = NextUnitState(unit);

		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
			fighter->nextState = NextFighterState(fighter);
	}
}


void BattleSimulator::AssignNextState()
{
	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
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
	unitRange.angleStart = unit->state.direction - 0.5f * unitRange.angleLength;

	unitRange.minimumRange = unit->stats.minimumRange;
	unitRange.maximumRange = unit->stats.maximumRange;

	unitRange.actualRanges.clear();

	if (unitRange.minimumRange > 0 && unitRange.maximumRange > 0)
	{
		float centerHeight = heightMap->InterpolateHeight(unitRange.center) + 1.9f;

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
				float height = heightMap->InterpolateHeight(unitRange.center + range * direction) + 0.5f;
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
	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
		bool isMissile = unit->stats.missileType != MissileType::None;
		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			Fighter* meleeTarget = fighter->state.meleeTarget;
			if (meleeTarget != nullptr)
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
	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
		bool controlsUnit = practice || currentPlayer == 0 || unit->player == currentPlayer;
		if (controlsUnit && unit->state.shootingCounter > unit->shootingCounter)
		{
			TriggerShooting(unit);
			unit->shootingCounter = unit->state.shootingCounter;
		}
	}

	ResolveProjectileCasualties();
}


void BattleSimulator::TriggerShooting(Unit* unit)
{
	if (unit->state.IsRouting())
		return;

	Shooting shooting;
	shooting.missileType = unit->stats.missileType;

	bool arq = shooting.missileType == MissileType::Arq;
	float distance = 0;

	for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
	{
		if (fighter->state.readyState == ReadyState_Prepared)
		{
			Projectile projectile;
			projectile.position1 = fighter->state.position;
			projectile.position2 = CalculateFighterMissileTarget(fighter);
			projectile.delay = (arq ? 0.5f : 0.2f) * ((rand() & 0x7FFF) / (float)0x7FFF);
			shooting.projectiles.push_back(projectile);
			distance += glm::length(projectile.position1 - projectile.position2) / unit->fightersCount;
		}
	}

	float speed = arq ? 750 : 75; // meters per second
	shooting.timeToImpact = distance / speed;

	shootings.push_back(shooting);
	recentShootings.push_back(shooting);
}


void BattleSimulator::ResolveProjectileCasualties()
{
	static int random = 0;

	for (std::vector<Shooting>::iterator s = shootings.begin(); s != shootings.end(); ++s)
	{
		Shooting& shooting = *s;

		shooting.timeToImpact -= timeStep;

		std::vector<Projectile>::iterator i = shooting.projectiles.begin();
		while (i != shooting.projectiles.end())
		{
			Projectile& projectile = *i;
			if (shooting.timeToImpact <= 0)
			{
				glm::vec2 hitpoint = projectile.position2;
				for (quadtree<Fighter*>::iterator j(_fighterQuadTree.find(hitpoint.x, hitpoint.y, 0.45f)); *j; ++j)
				{
					Fighter* fighter = **j;
					bool blocked = false;
					if (fighter->terrainForest)
						blocked = (random++ & 7) <= 5;
					if (!blocked)
						fighter->casualty = true;
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


void BattleSimulator::RemoveCasualties()
{
	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
		for (Fighter* fighter = unit->fighters, * end = fighter + unit->fightersCount; fighter != end; ++fighter)
		{
			if (fighter->state.opponent != nullptr && fighter->state.opponent->casualty)
				fighter->state.opponent = nullptr;
		}
	}

	bounds2f bounds = groundMap->GetBounds();
	glm::vec2 center = bounds.center();
	float radius = bounds.width() / 2;
	float radius_squared = radius * radius;


	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
		int index = 0;
		int n = unit->fightersCount;
		for (int j = 0; j < n; ++j)
		{
			if (unit->fighters[j].terrainImpassable && unit->state.IsRouting())
				unit->fighters[j].casualty = true;

			if (unit->fighters[j].casualty)
			{
				++unit->state.recentCasualties;

				for (BattleObserver* observer : _observers)
					observer->OnCasualty(unit, unit->fighters[j].state.position);
			}
			else
			{
				glm::vec2 diff = unit->fighters[j].state.position - center;
				if (glm::dot(diff, diff) < radius_squared)
				{
					if (index < j)
						unit->fighters[index].state = unit->fighters[j].state;
					unit->fighters[index].casualty = false;
					index++;
				}
			}
		}

		unit->fightersCount = index;
	}
}


void BattleSimulator::RemoveDeadUnits()
{
	std::vector<int> remove;
	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;
		if (unit->fightersCount == 0)
		{
			remove.push_back(unit->unitId);
		}
	}

	for (std::vector<int>::iterator i = remove.begin(); i != remove.end(); ++i)
	{
		int unitIndex = *i;
		units.erase(unitIndex);
	}

	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* unit = (*i).second;

		if (unit->command.missileTarget != nullptr && GetUnit(unit->command.missileTarget->unitId) == 0)
			unit->command.missileTarget = nullptr;

		if (unit->command.meleeTarget != nullptr && GetUnit(unit->command.meleeTarget->unitId) == 0)
			unit->command.meleeTarget = nullptr;
	}
}


UnitState BattleSimulator::NextUnitState(Unit* unit)
{
	UnitState result;

	result.center = unit->CalculateUnitCenter();
	result.direction = NextUnitDirection(unit);
	result.unitMode = NextUnitMode(unit);

	result.shootingCounter = unit->state.shootingCounter;

	if (unit->command.missileTargetLocked)
	{
		if (unit->command.missileTarget != nullptr && !IsWithinLineOfFire(unit, unit->command.missileTarget->state.center))
		{
			unit->command.missileTargetLocked = false;
			unit->command.missileTarget = nullptr;
		}
	}

	if (!unit->command.missileTargetLocked && !unit->command.holdFire)
	{
		unit->command.missileTarget = ClosestEnemyWithinLineOfFire(unit);
	}

	if (unit->state.unitMode != UnitMode_Standing || unit->command.missileTarget == nullptr)
	{
		result.loadingTimer = 0;
		result.loadingDuration = 0;
	}
	else if (unit->state.loadingTimer + timeStep < unit->state.loadingDuration)
	{
		result.loadingTimer = unit->state.loadingTimer + timeStep;
		result.loadingDuration = unit->state.loadingDuration;
	}
	else
	{
		if (unit->state.loadingDuration != 0
			&& unit->command.missileTarget != nullptr
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

	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* other = (*i).second;
		float distance = glm::length(other->state.center - unit->state.center);
		float weight = 1.0f * 50.0f / (distance + 50.0f);
		if (other->player == unit->player)
		{
			result.influence -= weight
					* (1 - other->state.morale)
					* (1 - unit->stats.trainingLevel)
					* other->stats.trainingLevel;
		}
	}

	if (winnerTeam != 0 && unit->team != winnerTeam)
	{
		result.morale = -1;
	}

	if (unit->fightersCount <= 8)
	{
		result.morale = -1;
	}

	if (practice && unit->team == 2 && unit->state.IsRouting())
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
	for (std::map<int, Unit*>::iterator i = units.begin(); i != units.end(); ++i)
	{
		Unit* target = (*i).second;
		if (target->player != unit->player && IsWithinLineOfFire(unit, target->state.center))
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
			if (glm::length(unit->state.center - unit->command.GetDestination()) > 8)
				return UnitMode_Moving;
			break;

		case UnitMode_Moving:
			if (glm::length(unit->state.center - unit->command.GetDestination()) <= 8)
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
		return unit->command.facing;
	else
		return unit->state.direction;
}


FighterState BattleSimulator::NextFighterState(Fighter* fighter)
{
	const FighterState& original = fighter->state;
	FighterState result;

	result.readyState = original.readyState;
	result.position = NextFighterPosition(fighter);
	result.position_z = heightMap->InterpolateHeight(result.position);
	result.velocity = NextFighterVelocity(fighter);


	// DIRECTION

	if (fighter->unit->state.unitMode == UnitMode_Moving)
	{
		result.direction = angle(original.velocity);
	}
	else if (original.opponent != nullptr)
	{
		result.direction = angle(original.opponent->state.position - original.position);
	}
	else
	{
		result.direction = fighter->unit->state.direction;
	}


	// OPPONENT

	if (original.opponent != 0 && glm::length(original.position - original.opponent->state.position) <= fighter->unit->stats.weaponReach * 2)
	{
		result.opponent = original.opponent;
	}
	else if (fighter->unit->state.unitMode != UnitMode_Moving && !fighter->unit->state.IsRouting())
	{
		result.opponent = FindFighterStrikingTarget(fighter);
	}

	// DESTINATION

	if (original.opponent != nullptr)
	{
		result.destination = original.opponent->state.position
				- fighter->unit->stats.weaponReach * vector2_from_angle(original.direction);
	}
	else
	{
		switch (original.readyState)
		{
			case ReadyState_Unready:
			case ReadyState_Readying:
			case ReadyState_Prepared:
				result.destination = MovementRules::NextFighterDestination(fighter);
				break;

			default:
				result.destination = original.position;
				break;
		}
	}

	// READY STATE

	switch (original.readyState)
	{
		case ReadyState_Unready:
			if (fighter->unit->command.meleeTarget != nullptr)
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
			if (original.readyingTimer > timeStep)
			{
				result.readyingTimer = original.readyingTimer - timeStep;
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
			else if (result.opponent != nullptr)
			{
				result.readyState = ReadyState_Striking;
				result.strikingTimer = fighter->unit->stats.strikingDuration;
			}
			break;

		case ReadyState_Striking:
			if (original.strikingTimer > timeStep)
			{
				result.strikingTimer = original.strikingTimer - timeStep;
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
			if (original.stunnedTimer > timeStep)
			{
				result.stunnedTimer = original.stunnedTimer - timeStep;
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
		glm::vec2 result = fighter->state.position + fighter->state.velocity * timeStep;
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
				if (glm::dot(diff, diff) < fighterDistance * fighterDistance)
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
			if (obstacle->unit->player != unit->player)
			{
				glm::vec2 r = obstacle->unit->stats.weaponReach * vector2_from_angle(obstacle->state.direction);
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

	if (glm::length(fighter->state.position - fighter->terrainPosition) > 4)
	{
		fighter->terrainForest = groundMap->IsForest(fighter->state.position);
		fighter->terrainImpassable = groundMap->IsImpassable(fighter->state.position);
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
	if (diff_len < 0.01)
		return diff;

	glm::vec2 delta = glm::normalize(diff) * speed;
	float delta_len = glm::dot(delta, delta);

	return delta_len < diff_len ? delta : diff;
}


Fighter* BattleSimulator::FindFighterStrikingTarget(Fighter* fighter)
{
	Unit* unit = fighter->unit;

	glm::vec2 position = fighter->state.position + unit->stats.weaponReach * vector2_from_angle(fighter->state.direction);
	float radius = 1.1f;

	for (quadtree<Fighter*>::iterator i(_fighterQuadTree.find(position.x, position.y, radius)); *i; ++i)
	{
		Fighter* target = **i;
		if (target != fighter && target->unit->player != unit->player)
		{
			return target;
		}
	}

	return 0;
}


glm::vec2 BattleSimulator::CalculateFighterMissileTarget(Fighter* fighter)
{
	Unit* unit = fighter->unit;

	if (unit->command.missileTarget != nullptr)
	{
		float dx = 10.0f * ((rand() & 255) / 128.0f - 1.0f);
		float dy = 10.0f * ((rand() & 255) / 127.0f - 1.0f);
		return unit->command.missileTarget->state.center + glm::vec2(dx, dy);
	}

	return glm::vec2();
}
