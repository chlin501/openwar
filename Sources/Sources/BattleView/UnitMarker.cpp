// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "UnitMarker.h"
#include "../../BattleModel/BattleSimulator.h"



UnitMarker::UnitMarker(BattleView* battleView, Unit* unit) :
_battleView(battleView),
_unit(unit)
{
}


UnitMarker::~UnitMarker()
{
}
