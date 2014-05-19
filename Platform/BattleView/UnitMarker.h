// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMarker_H
#define UnitMarker_H

class BattleView;
class Unit;


class UnitMarker
{
protected:
	BattleView* _battleView;
	Unit* _unit;

public:
	UnitMarker(BattleView* battleView, Unit* unit);
	virtual ~UnitMarker();

	Unit* GetUnit() const { return _unit; }
};



#endif
