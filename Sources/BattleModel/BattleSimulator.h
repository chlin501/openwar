// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SIMULATIONRULES_H
#define SIMULATIONRULES_H

#include <set>
#include "../Library/Algorithms/quadtree.h"
#include "BattleModel.h"

class Fighter;
class Unit;
class BattleView;


class BattleObserver
{
public:
	virtual ~BattleObserver();

	virtual void OnNewUnit(Unit* unit) = 0;
	virtual void OnShooting(const Shooting& shooting) = 0;
	virtual void OnCasualty(const Casualty& casualty) = 0;
};


class BattleSimulator
{
	BattleModel* _battleModel;
	quadtree<Fighter*> _fighterQuadTree;
	quadtree<Fighter*> _weaponQuadTree;
	float _secondsSinceLastTimeStep;
	std::set<BattleObserver*> _observers;

public:
	Player currentPlayer;
	bool practice;
	std::vector<Shooting> recentShootings;
	std::vector<Casualty> recentCasualties;

	BattleSimulator(BattleModel* battleModel);

	BattleModel* GetBattleModel() const { return _battleModel; }

	void AddObserver(BattleObserver* observer);
	void RemoveObserver(BattleObserver* observer);
	const std::set<BattleObserver*>& GetObservers() const;

	Unit* AddUnit(Player player, const char* unitClass, int numberOfFighters, UnitStats stats, glm::vec2 position);

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
