// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleScript_H
#define BattleScript_H

#include <map>
#include "BattleSimulator.h"

class BattleScenario;


class BattleScript : public BattleObserver
{
protected:
	BattleScenario* _scenario{};
	BattleSimulator* _simulator{};
	std::map<int, Unit*> _units{};
	std::map<Unit*, int> _unitId{};
	int _nextUnitId{};

public:
	BattleScript(BattleScenario* scenario);
	virtual ~BattleScript();

	virtual void Execute();
	virtual void Tick(double secondsSinceLastTick);

	// BattleObserver
	virtual void OnSetGroundMap(GroundMap* groundMap);
	virtual void OnAddUnit(Unit* unit);
	virtual void OnRemoveUnit(Unit* unit);
	virtual void OnCommand(Unit* unit, float timer);
	virtual void OnShooting(const Shooting& shooting, float timer);
	virtual void OnRelease(const Shooting& shooting);
	virtual void OnCasualty(const Fighter& fighter);
	virtual void OnRouting(Unit* unit);

protected:
	int NewUnit(int commanderId, const char* unitClass, int strength, glm::vec2 position, float bearing);
};


#endif
