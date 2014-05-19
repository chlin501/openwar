// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RangeMarker_H
#define RangeMarker_H

#include "BattleModel/BattleSimulator.h"

class GradientTriangleStripShape3;


class RangeMarker
{
public:
	BattleSimulator* _battleSimulator;
	Unit* _unit;

public:
	RangeMarker(BattleSimulator* battleSimulator, Unit* unit);

	void Render(GradientTriangleStripShape3* renderer);

private:
	void RenderMissileRange(GradientTriangleStripShape3* renderer, const UnitRange& unitRange);
	void RenderMissileTarget(GradientTriangleStripShape3* renderer, glm::vec2 target);

	glm::vec3 GetPosition(glm::vec2 p) const;
};


#endif
