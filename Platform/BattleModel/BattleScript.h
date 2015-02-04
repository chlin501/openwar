// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleScript_H
#define BattleScript_H

#include <map>
#include "BattleSimulator.h"

class BattleScenario;


class BattleScript : BattleObserver
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

private: // BattleObserver
	void OnSetGroundMap(GroundMap* groundMap) override;
	void OnAddUnit(Unit* unit) override;
	void OnRemoveUnit(Unit* unit) override;
	void OnCommand(Unit* unit, float timer) override;
	void OnShooting(const Shooting& shooting, float timer) override;
	void OnRelease(const Shooting& shooting) override;
	void OnCasualty(const Fighter& fighter) override;
	void OnRouting(Unit* unit) override;

protected:
	int NewUnit(int commanderId, const char* unitClass, int strength, glm::vec2 position, float bearing);
};


#endif
