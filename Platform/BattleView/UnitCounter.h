// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitCounter_H
#define UnitCounter_H

#include "BattleModel/BattleSimulator.h"
#include "BattleModel/SamuraiModule.h"
#include "Algebra/bounds.h"
#include "VertexBuffer.h"

class BattleSimulator;
class BattleView;
class BillboardModel;
class TextureBillboardRenderer;
class TextureTriangleShape3;
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

	void AppendUnitMarker(TextureBillboardRenderer* renderer, bool flip);
	void AppendFacingMarker(TextureTriangleShape3* renderer, BattleView* battleView);

	void AppendFighterWeapons(VertexBuffer_3f* vertices);
	void AppendFighterBillboards(BillboardModel* billboardModel);
};


#endif
