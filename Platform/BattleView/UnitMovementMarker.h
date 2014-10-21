// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitMovementMarker_H
#define UnitMovementMarker_H

#include "UnitMarker.h"
#include "VertexBuffer.h"

class BattleView;
class TextureBillboardRenderer;


class UnitMovementMarker : public UnitMarker
{
public:
	UnitMovementMarker(BattleView* battleView, Unit* unit);
	~UnitMovementMarker();

	bool Animate(float seconds);

	void RenderMovementMarker(TextureBillboardRenderer* renderer);
	void AppendFacingMarker(VertexBuffer_3f_2f* vertices, BattleView* battleView);
	void RenderMovementFighters(VertexBuffer_3f_4f_1f* vertices);
	void RenderMovementPath(VertexBuffer_3f_4f* vertices);
};


#endif
