// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMovementMarker_H
#define UnitMovementMarker_H

#include "UnitMarker.h"
#include "Shapes/VertexShape.h"

class BattleView;
class BillboardTextureShape;


class UnitMovementMarker : public UnitMarker
{
public:
	UnitMovementMarker(BattleView* battleView, BattleObjects::Unit* unit);
	~UnitMovementMarker();

	bool Animate(float seconds);

	void RenderMovementMarker(BillboardTextureShape* renderer);
	void AppendFacingMarker(VertexShape_2f_2f* vertices, BattleView* battleView);
	void RenderMovementFighters(VertexShape_3f_4f_1f* vertices);
	void RenderMovementPath(VertexShape_3f_4f* vertices);
};


#endif
