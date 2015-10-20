// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleScript_H
#define BattleScript_H


class BattleScript
{
public:
	virtual ~BattleScript();

	virtual void Tick(double secondsSinceLastTick) = 0;
};


#endif
