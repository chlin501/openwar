// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMarker_H
#define UnitMarker_H

class BattleSimulator;
class Unit;


class UnitMarker
{
protected:
	BattleSimulator* _battleSimulator;
	Unit* _unit;

public:
	UnitMarker(BattleSimulator* battleSimulator, Unit* unit);
	virtual ~UnitMarker();

	Unit* GetUnit() const { return _unit; }
};



#endif
