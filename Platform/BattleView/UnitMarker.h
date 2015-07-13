// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMarker_H
#define UnitMarker_H

#include "BattleObjects_v1.h"

class BattleView;


class UnitMarker
{
protected:
	BattleView* _battleView{};
	BattleObjects_v1::Unit* _unit{};

public:
	UnitMarker(BattleView* battleView, BattleObjects_v1::Unit* unit);
	virtual ~UnitMarker();

	BattleObjects_v1::Unit* GetUnit() const { return _unit; }
};



#endif
