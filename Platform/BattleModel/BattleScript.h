// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleScript_H
#define BattleScript_H

#include <map>
#include "BattleSimulator.h"


class BattleScript
{
protected:
	BattleSimulator* _simulator{};

public:
	BattleScript(BattleSimulator* simulator);
	virtual ~BattleScript();

	virtual void Execute();
	virtual void Tick(double secondsSinceLastTick);
};


#endif
