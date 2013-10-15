// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "UnitMarker.h"
#include "BattleSimulator.h"



UnitMarker::UnitMarker(BattleSimulator* battleSimulator, Unit* unit) :
_battleSimulator(battleSimulator),
_unit(unit)
{
}


UnitMarker::~UnitMarker()
{
}
