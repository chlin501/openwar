// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleSimulator_H
#define BattleSimulator_H

#include <map>
#include <set>
#include <string>
#include "../Library/Algorithms/quadtree.h"
#include "MovementRules.h"

class Fighter;
class Unit;
class BattleView;
class BattleSimulator;
class HeightMap;
class GroundMap;

struct Fighter;
struct Unit;
struct UnitUpdate;


enum class PlatformType
{
	None,
	Infantry,
	Cavalry
};


enum class MissileType
{
	None,
	Bow,
	Arq
};


enum UnitMode
{
	UnitMode_Initializing,
	UnitMode_Standing,
	UnitMode_Moving,
	UnitMode_Turning
};


struct Projectile
{
	glm::vec2 position1;
	glm::vec2 position2;
	float delay;

	Projectile() :
	position1(),
	position2(),
	delay(0)
	{
	}
};


struct Shooting
{
	MissileType missileType;
	float timeToImpact;
	std::vector<Projectile> projectiles;

	Shooting() :
	missileType(MissileType::None),
	timeToImpact(0),
	projectiles()
	{
	}
};


enum ReadyState
{
	ReadyState_Unready,
	ReadyState_Readying,
	ReadyState_Prepared,
	ReadyState_Striking,
	ReadyState_Stunned
};


struct FighterState
{
	// dynamic attributes
	glm::vec2 position;
	float position_z;
	ReadyState readyState;
	float readyingTimer;
	float strikingTimer;
	float stunnedTimer;
	Fighter* opponent;

	// intermediate attributes
	glm::vec2 destination;
	glm::vec2 velocity;
	float direction;
	Fighter* meleeTarget;

	FighterState() :
	position(),
	position_z(0),
	readyState(ReadyState_Unready),
	readyingTimer(0),
	strikingTimer(0),
	stunnedTimer(0),
	opponent(nullptr),
	destination(),
	velocity(),
	direction(0),
	meleeTarget(nullptr)
	{
	}
};


struct Fighter
{
	// static attributes
	Unit* unit;

	// dynamic attributes
	FighterState state;

	// optimization attributes
	bool terrainForest;
	bool terrainImpassable;
	glm::vec2 terrainPosition;

	// intermediate attributes
	FighterState nextState;
	bool casualty;


	Fighter() :
	unit(nullptr),
	state(),
	terrainForest(false),
	terrainImpassable(false),
	terrainPosition(),
	nextState(),
	casualty(false)
	{
	}
};


struct UnitStats
{
	PlatformType platformType;
	MissileType missileType;

	float weaponReach;
	float trainingLevel;
	float strikingDuration;
	float readyingDuration;
	float fireRate; // shots per minute
	float minimumRange; // minimum fire range in meters
	float maximumRange; // maximum fire range in meters
	float fireAccuracy; // percentage of hit at half range
	float walkingSpeed; // meters per second
	float runningSpeed; // meters per second
	glm::vec2 fighterSize; // x = side-to-side, y = front-to-back
	glm::vec2 spacing;  // x = side-to-side, y = front-to-back

	UnitStats() :
	platformType(PlatformType::None),
	missileType(MissileType::None),
	weaponReach(0),
	trainingLevel(0),
	strikingDuration(0),
	fireRate(25.0f), // rounds per minute
	minimumRange(0),
	maximumRange(0),
	fireAccuracy(0.7f),
	walkingSpeed(0),
	runningSpeed(0),
	fighterSize(glm::vec2(10, 10)),
	spacing(glm::vec2(10, 10))
	{
	}
};


struct UnitRange
{
	glm::vec2 center;
	float angleStart;
	float angleLength;
	float minimumRange;
	float maximumRange;
	std::vector<float> actualRanges;

	UnitRange() :
	center(),
	angleStart(0),
	angleLength(0),
	minimumRange(0),
	maximumRange(0),
	actualRanges()
	{

	}

	bool IsWithinRange(glm::vec2 p) const;
};


struct UnitState
{
	// dynamic attributes
	float loadingTimer;
	float loadingDuration;
	int shootingCounter;
	float morale;

	// optimization attributes
	UnitMode unitMode;
	glm::vec2 center;
	float direction;
	float influence;

	// intermediate attributes
	int recentCasualties;

	glm::vec2 waypoint;


	bool IsRouting() const
	{
		float x = morale + influence;
		return x <= 0;
	}

	float GetRoutingBlinkTime() const
	{
		float x = morale + influence;
		return 0 <= x && x < 0.33f ? 0.1f + x * 3 : 0;
	}

	UnitState() :
	loadingTimer(0),
	loadingDuration(0),
	shootingCounter(0),
	morale(1),
	unitMode(UnitMode_Initializing),
	center(),
	direction(0),
	influence(0),
	recentCasualties(0),
	waypoint()
	{
	}
};



struct UnitCommand
{
	std::vector<glm::vec2> path;
	float facing;
	bool running;
	Unit* meleeTarget;
	Unit* missileTarget;
	bool missileTargetLocked; // updated by TouchGesture()
	bool holdFire;

	UnitCommand() :
	path(),
	facing(0),
	running(false),
	meleeTarget(nullptr),
	missileTarget(nullptr),
	missileTargetLocked(false),
	holdFire(false)
	{
	}


	void UpdatePath(glm::vec2 curr, glm::vec2 dest)
	{
		MovementRules::UpdateMovementPath(path, curr, dest);
	}

	void ClearPathAndSetDestination(glm::vec2 p)
	{
		path.clear();
		path.push_back(p);
	}


	glm::vec2 GetDestination()
	{
		return !path.empty() != 0 ? path.back() : glm::vec2();
	}
};


struct Unit
{
	// static attributes
	int unitId;
	int player;
	int team;

	std::string unitClass;
	UnitStats stats;
	Fighter* fighters;

	// dynamic attributes
	UnitState state; // updated by AssignNextState()
	int fightersCount; // updated by RemoveCasualties()
	int shootingCounter; // updated by ResolveMissileCombat()
	Formation formation; // updated by UpdateFormation()
	float timeUntilSwapFighters;

	// intermediate attributes
	UnitState nextState; // updated by ComputeNextState()
	UnitRange unitRange;

	// control attributes
	UnitCommand command; // updated by TouchGesture()

	Unit() :
	unitId(0),
	player(1),
	team(1),
	stats(),
	fighters(nullptr),
	state(),
	fightersCount(0),
	shootingCounter(0),
	formation(),
	timeUntilSwapFighters(0),
	nextState(),
	unitRange(),
	command()
	{
	}

	glm::vec2 CalculateUnitCenter();

	float GetSpeed();

	static int GetFighterRank(Fighter* fighter);
	static int GetFighterFile(Fighter* fighter);
	static Fighter* GetFighter(Unit* unit, int rank, int file);
};



class BattleObserver
{
public:
	virtual ~BattleObserver();

	virtual void OnNewUnit(Unit* unit) = 0;
	virtual void OnShooting(const Shooting& shooting) = 0;
	virtual void OnCasualty(Unit* unit, glm::vec2 position) = 0;
};


class BattleSimulator
{
	quadtree<Fighter*> _fighterQuadTree;
	quadtree<Fighter*> _weaponQuadTree;
	float _secondsSinceLastTimeStep;
	std::set<BattleObserver*> _observers;

public:
	bool practice;
	std::vector<Shooting> recentShootings;

	GroundMap* groundMap;
	HeightMap* heightMap;

	int currentPlayer;
	int lastUnitId;
	int blueTeam;
	int winnerTeam;
	float time;
	float timeStep;

	std::map<int, Unit*> units;
	std::vector<Shooting> shootings;

public:
	BattleSimulator();
	~BattleSimulator();

	void AddObserver(BattleObserver* observer);
	void RemoveObserver(BattleObserver* observer);
	const std::set<BattleObserver*>& GetObservers() const;


	Unit* GetUnit(int unitId) const
	{
		std::map<int, Unit*>::const_iterator i = units.find(unitId);
		return i != units.end() ? (*i).second : 0;
	}

	bool IsMelee() const;

	Unit* AddUnit(int player, int team, const char* unitClass, int numberOfFighters, UnitStats stats, glm::vec2 position);

	void AdvanceTime(float secondsSinceLastTime);

private:
	void SimulateOneTimeStep();

	void RebuildQuadTree();

	void ComputeNextState();
	void AssignNextState();
	void UpdateUnitRange(Unit* unit);

	void ResolveMeleeCombat();
	void ResolveMissileCombat();

	void TriggerShooting(Unit* unit);
	void ResolveProjectileCasualties();

	void RemoveCasualties();
	void RemoveDeadUnits();

	UnitState NextUnitState(Unit* unit);
	UnitMode NextUnitMode(Unit* unit);
	float NextUnitDirection(Unit* unit);

	FighterState NextFighterState(Fighter* fighter);
	glm::vec2 NextFighterPosition(Fighter* fighter);
	glm::vec2 NextFighterVelocity(Fighter* fighter);

	Fighter* FindFighterStrikingTarget(Fighter* fighter);
	glm::vec2 CalculateFighterMissileTarget(Fighter* fighter);

	bool IsWithinLineOfFire(Unit* unit, glm::vec2 position);
	Unit* ClosestEnemyWithinLineOfFire(Unit* unit);
};



#endif
