// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitCounter_H
#define UnitCounter_H

#include "BattleSimulator_v1_0_0.h"
#include "BattleModel/SamuraiModule.h"
#include "Algebra/bounds.h"
#include "Shapes/VertexShape.h"

class BattleView;
class BillboardModel;
class BillboardTextureShape;


class UnitCounter
{
public:
	BattleView* _battleView{};
	BattleObjects::Unit* _unit{};
	float _routingTimer{};
	SamuraiWeapon _samuraiWeapon{};
	SamuraiPlatform _samuraiPlatform{};

public:
	UnitCounter(BattleView* battleView, BattleObjects::Unit* unit);
	~UnitCounter();

	BattleObjects::Unit* GetUnit() const { return _unit; }

	bool Animate(float seconds);

	void AppendUnitMarker(BillboardTextureShape* renderer, bool flip);
	void AppendFacingMarker(VertexShape_2f_2f* vertices, BattleView* battleView);

	void AppendFighterWeapons(VertexShape_3f* vertices);
	void AppendFighterBillboards(BillboardModel* billboardModel);

private:
	static float GetRoutingBlinkTime(BattleObjects::Unit* unit);
};


#endif
