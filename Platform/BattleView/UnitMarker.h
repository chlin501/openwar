// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMarker_H
#define UnitMarker_H

#include "BattleModel/BattleObjects_v1.h"

class BattleView;


class UnitMarker
{
protected:
	BattleView* _battleView{};
	BattleObjects::Unit* _unit{};

public:
	UnitMarker(BattleView* battleView, BattleObjects::Unit* unit);
	virtual ~UnitMarker();

	BattleObjects::Unit* GetUnit() const { return _unit; }
};



#endif
