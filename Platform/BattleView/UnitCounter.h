// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitCounter_H
#define UnitCounter_H

#include "BattleModel/BattleSimulator.h"
#include "BattleModel/SamuraiModule.h"
#include "Algebra/bounds.h"
#include "Shapes/VertexShape.h"

class BattleSimulator;
class BattleView;
class BillboardModel;
class BillboardTextureShape;
class Unit;


class UnitCounter
{
public:
	BattleView* _battleView;
	Unit* _unit;
	float _routingTimer;
	int _weaponIndex;
	SamuraiWeapon _samuraiWeapon;
	SamuraiPlatform _samuraiPlatform;

public:
	UnitCounter(BattleView* battleView, Unit* unit);
	~UnitCounter();

	Unit* GetUnit() const { return _unit; }

	bool Animate(float seconds);

	void AppendUnitMarker(BillboardTextureShape* renderer, bool flip);
	void AppendFacingMarker(VertexShape_2f_2f* vertices, BattleView* battleView);

	void AppendFighterWeapons(VertexShape_3f* vertices);
	void AppendFighterBillboards(BillboardModel* billboardModel);
};


#endif
