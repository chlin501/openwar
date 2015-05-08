// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleScript_H
#define BattleScript_H

#include <map>
#include "BattleSimulator.h"

class BattleScenario;


class BattleScript
{
protected:
	BattleScenario* _scenario{};
	BattleSimulator* _simulator{};

public:
	BattleScript(BattleScenario* scenario);
	virtual ~BattleScript();

	virtual void Execute();
	virtual void Tick(double secondsSinceLastTick);

protected:
	void NewUnit(int commanderId, const char* unitClass, int strength, glm::vec2 position, float bearing);
};


#endif
