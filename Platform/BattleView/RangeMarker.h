// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RangeMarker_H
#define RangeMarker_H

#include "BattleSimulator_v1_0_0.h"
#include "Shapes/VertexShape.h"


class RangeMarker
{
public:
	BattleSimulator_v1_0_0* _battleSimulator{};
	BattleObjects_v1::Unit* _unit{};

public:
	RangeMarker(BattleSimulator_v1_0_0* battleSimulator, BattleObjects_v1::Unit* unit);

	void Render(VertexShape_3f_4f* vertices);

private:
	void RenderMissileRange(VertexShape_3f_4f* vertices, const BattleObjects_v1::UnitRange& unitRange);
	void RenderMissileTarget(VertexShape_3f_4f* vertices, glm::vec2 target);

	glm::vec3 GetPosition(glm::vec2 p) const;
};


#endif
