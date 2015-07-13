// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleSimulator_H
#define BattleSimulator_H

#include <map>
#include <set>
#include <string>
#include "Algorithms/quadtree.h"
#include "BattleMap/GroundMap.h"
#include "BattleObjects_v1.h"

class BattleObserver;



class BattleSimulator
{
	std::set<BattleObserver*> _observers{};
	quadtree<Fighter*> _fighterQuadTree{0, 0, 1024, 1024};
	quadtree<Fighter*> _weaponQuadTree{0, 0, 1024, 1024};

	BattleMap* _battleMap{};

	std::vector<Unit*> _units{};
	std::vector<std::pair<float, Shooting>> _shootings{};
	std::map<int, int> _kills{};

	float _secondsSinceLastTimeStep{};
	float _timeStep{1.0f / 15.0f};
	bool _practice{};
	int _winnerTeam{};
	glm::vec3 _deploymentZones[2]{};

	/***/
	BattleScript* _script{};
	std::vector<BattleObjects::Commander*> _commanders{};
	BattleObjects::Commander* _dummyCommander{};
	int _teamPosition1{};
	int _teamPosition2{};
	float _deploymentTimer{};
	bool _deploymentEnabled{};

public:
	BattleSimulator(BattleMap* battleMap);
	~BattleSimulator();

	float GetTimeStep() const { return _timeStep; }
	float GetTimerDelay() const { return 0.25f; }

	void SetPractice(bool value) { _practice = value; }
	int GetKills(int team) { return _kills[team]; }

	void AddObserver(BattleObserver* observer);
	void RemoveObserver(BattleObserver* observer);

	BattleMap* GetBattleMap() const;

	void SetDeploymentZone(int team, glm::vec2 center, float radius);
	glm::vec2 GetDeploymentCenter(int team) const;
	float GetDeploymentRadius(int team) const;
	bool IsDeploymentZone(int team, glm::vec2 position) const;
	glm::vec2 ConstrainDeploymentZone(int team, glm::vec2 position, float inset) const;
	bool HasCompletedDeployment(int team) const;
	void Deploy(Unit* unit, glm::vec2 position);

	const std::vector<Unit*>& GetUnits() { return _units; }
	Unit* AddUnit(BattleObjects::Commander* commander, const char* unitClass, int numberOfFighters, UnitStats stats, glm::vec2 position);
	void RemoveUnit(Unit* unit);

	void NewUnit(int commanderId, const char* unitClass, int strength, glm::vec2 position, float bearing);

	void SetUnitCommand(Unit* unit, const UnitCommand& command, float timer);

	void AddShooting(const Shooting& shooting, float timer);

	void AdvanceTime(float secondsSinceLastTime);

	int CountCavalryInMelee() const;
	int CountInfantryInMelee() const;

	int GetWinnerTeam() const { return _winnerTeam; }

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
	void RemoveFinishedShootings();

	UnitState NextUnitState(Unit* unit);
	UnitMode NextUnitMode(Unit* unit);
	float NextUnitDirection(Unit* unit);

	FighterState NextFighterState(Fighter* fighter);
	glm::vec2 NextFighterPosition(Fighter* fighter);
	glm::vec2 NextFighterVelocity(Fighter* fighter);

	Fighter* FindFighterStrikingTarget(Fighter* fighter);

	bool IsWithinLineOfFire(Unit* unit, glm::vec2 position);
	Unit* ClosestEnemyWithinLineOfFire(Unit* unit);

	bool TeamHasAbandondedBattle(int team) const;

public:
	void SetScript(BattleScript* value);

	void SetTeamPosition(int team, int position);
	int GetTeamPosition(int team) const;

	BattleObjects::Commander* AddCommander(const char* playerId, int team, BattleObjects::CommanderType type);
	BattleObjects::Commander* GetCommander(const char* playerId) const;
	const std::vector<BattleObjects::Commander*>& GetCommanders() const { return _commanders; }
	BattleObjects::Commander* GetDummyCommander() const;

	void Tick(double secondsSinceLastTick);

	void EnableDeploymentZones(float deploymentTimer);
	void UpdateDeploymentZones(double secondsSinceLastTick);
};




class MovementRules
{
public:
	static void AdvanceTime(Unit* unit, float timeStep);
	static void SwapFighters(Unit* unit);
	static glm::vec2 NextFighterDestination(Fighter* fighter);
	static glm::vec2 NextWaypoint(Unit* unit);
};


#endif
