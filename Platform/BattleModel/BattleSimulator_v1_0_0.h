// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleSimulator_v1_0_0_H
#define BattleSimulator_v1_0_0_H

#include <map>
#include <set>
#include <string>
#include "Algorithms/quadtree.h"
#include "BattleMap/GroundMap.h"
#include "BattleObjects_v1.h"
#include "BattleSimulator.h"



class BattleSimulator_v1_0_0 : public BattleSimulator, public BattleObjects_v1
{
	quadtree<BattleObjects_v1::Fighter*> _fighterQuadTree{0, 0, 1024, 1024};
	quadtree<BattleObjects_v1::Fighter*> _weaponQuadTree{0, 0, 1024, 1024};

	std::vector<std::pair<float, BattleObjects::Shooting>> _shootings{};
	std::map<int, int> _kills{};

	float _secondsSinceLastTimeStep{};
	float _timeStep{1.0f / 15.0f};

public:
	BattleSimulator_v1_0_0(BattleMap* battleMap);
	~BattleSimulator_v1_0_0();

	void AdvanceTime(float secondsSinceLastTime) override;

	int GetKills(int team) { return _kills[team]; }

	BattleObjects::Unit* AddUnit(BattleCommander* commander, const char* unitClass, int numberOfFighters, glm::vec2 position, float bearing) override;
	void DeployUnit(BattleObjects::Unit* unit, glm::vec2 position, float bearing) override;
	void RemoveUnit(BattleObjects::Unit* unit) override;

	void SetUnitCommand(BattleObjects::Unit* unit, const BattleObjects::UnitCommand& command, float timer) override;
	void IssueUnitCommand(BattleObjects::Unit* unit, const BattleObjects::UnitCommand& command, float timer) override;

	void AddShooting(const BattleObjects::Shooting& shooting, float timer);

private:
	void SimulateOneTimeStep();

	void RebuildQuadTree();

	void ComputeNextState();
	void AssignNextState();
	void UpdateUnitRange(BattleObjects_v1::Unit* unit);

	void ResolveMeleeCombat();
	void ResolveMissileCombat();

	void TriggerShooting(BattleObjects_v1::Unit* unit);
	void ResolveProjectileCasualties();

	void RemoveCasualties();
	void RemoveDeadUnits();
	void RemoveFinishedShootings();

	BattleObjects_v1::UnitState NextUnitState(BattleObjects_v1::Unit* unit);
	BattleObjects_v1::UnitMode NextUnitMode(BattleObjects_v1::Unit* unit);
	float NextUnitDirection(BattleObjects_v1::Unit* unit);

	BattleObjects_v1::FighterState NextFighterState(BattleObjects_v1::Fighter* fighter);
	glm::vec2 NextFighterPosition(BattleObjects_v1::Fighter* fighter);
	glm::vec2 NextFighterVelocity(BattleObjects_v1::Fighter* fighter);

	BattleObjects_v1::Fighter* FindFighterStrikingTarget(BattleObjects_v1::Fighter* fighter);

	bool IsWithinLineOfFire(BattleObjects_v1::Unit* unit, glm::vec2 position);
	BattleObjects_v1::Unit* ClosestEnemyWithinLineOfFire(BattleObjects_v1::Unit* unit);

	static void MovementRules_AdvanceTime(BattleObjects_v1::Unit* unit, float timeStep);
	static void MovementRules_SwapFighters(BattleObjects_v1::Unit* unit);
	static glm::vec2 MovementRules_NextFighterDestination(BattleObjects_v1::Fighter* fighter);
	static glm::vec2 MovementRules_NextWaypoint(BattleObjects_v1::Unit* unit);
};


#endif
