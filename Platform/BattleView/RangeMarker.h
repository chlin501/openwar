// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RangeMarker_H
#define RangeMarker_H

#include "BattleModel/BattleSimulator.h"
#include "Shapes/VertexShape.h"


class RangeMarker
{
public:
	BattleSimulator* _battleSimulator{};
	Unit* _unit{};

public:
	RangeMarker(BattleSimulator* battleSimulator, Unit* unit);

	void Render(VertexShape_3f_4f* vertices);

private:
	void RenderMissileRange(VertexShape_3f_4f* vertices, const UnitRange& unitRange);
	void RenderMissileTarget(VertexShape_3f_4f* vertices, glm::vec2 target);

	glm::vec3 GetPosition(glm::vec2 p) const;
};


#endif
