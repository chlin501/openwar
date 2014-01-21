// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/constants.hpp>

#include "RangeMarker.h"
#include "../../BattleModel/GroundMap.h"
#include "../../BattleModel/HeightMap.h"
#include "../../BattleModel/SmoothGroundMap.h"
#include "../../Library/Algebra/geometry.h"
#include "GradientShape3.h"


RangeMarker::RangeMarker(BattleSimulator* battleSimulator, Unit* unit) :
_battleSimulator(battleSimulator),
_unit(unit)
{
}


void RangeMarker::Render(GradientTriangleStripShape3* renderer)
{
	const UnitCommand& command = _unit->GetCommand();
	if (command.missileTarget != nullptr)
	{
		RenderMissileTarget(renderer, command.missileTarget->state.center);
	}
	else if (_unit->unitRange.maximumRange > 0 && _unit->state.unitMode != UnitMode_Moving && !_unit->state.IsRouting())
	{
		RenderMissileRange(renderer, _unit->unitRange);
	}
}


void RangeMarker::RenderMissileRange(GradientTriangleStripShape3* renderer, const UnitRange& unitRange)
{
	glm::vec2 center = unitRange.center;
	const float thickness = 8;

	glm::vec4 c0 = glm::vec4(255, 64, 64, 0) / 255.0f;
	glm::vec4 c1 = glm::vec4(255, 64, 64, 24) / 255.0f;

	float angleMinOuter = unitRange.angleStart;
	float angleMinInner = angleMinOuter + 0.03f;
	float angleMaxOuter = unitRange.angleStart + unitRange.angleLength;
	float angleMaxInner = angleMaxOuter - 0.03f;

	float range = unitRange.actualRanges.front();
	glm::vec2 p2 = range * vector2_from_angle(angleMinOuter);
	glm::vec2 p4 = range * vector2_from_angle(angleMinInner);
	glm::vec2 p5 = (range - thickness) * vector2_from_angle(angleMinInner);
	glm::vec2 p1 = unitRange.minimumRange * vector2_from_angle(angleMinOuter);
	glm::vec2 p3 = p1 + (p4 - p2);


	for (int i = 0; i <= 8; ++i)
	{
		float t = i / 8.0f;
		renderer->AddVertex(GetPosition(center + glm::mix(p3, p5, t)), c0);
		renderer->AddVertex(GetPosition(center + glm::mix(p1, p2, t)), c1);
	}

	renderer->AddVertex(GetPosition(center + p4), c1);
	renderer->AddVertex(GetPosition(center + p4), c1);
	renderer->AddVertex(GetPosition(center + p5), c0);

	int n = (int)unitRange.actualRanges.size();
	float angleDelta = (angleMaxInner - angleMinInner) / (n - 1);
	for (int i = 0; i < n; ++i)
	{
		range = unitRange.actualRanges[i];
		float angle = angleMinInner + i * angleDelta;
		renderer->AddVertex(GetPosition(center + (range - thickness) * vector2_from_angle(angle)), c0);
		renderer->AddVertex(GetPosition(center + range * vector2_from_angle(angle)), c1);
	}

	range = unitRange.actualRanges.back();
	p2 = range * vector2_from_angle(angleMaxOuter);
	p4 = range * vector2_from_angle(angleMaxInner);
	p5 = (range - thickness) * vector2_from_angle(angleMaxInner);
	p1 = unitRange.minimumRange * vector2_from_angle(angleMaxOuter);
	p3 = p1 + (p4 - p2);

	renderer->AddVertex(GetPosition(center + p4), c1);
	for (int i = 0; i <= 8; ++i)
	{
		float t = i / 8.0f;
		renderer->AddVertex(GetPosition(center + glm::mix(p2, p1, t)), c1);
		renderer->AddVertex(GetPosition(center + glm::mix(p5, p3, t)), c0);
	}
}



void RangeMarker::RenderMissileTarget(GradientTriangleStripShape3* renderer, glm::vec2 target)
{
	glm::vec4 c0 = glm::vec4(255, 64, 64, 0) / 255.0f;
	glm::vec4 c1 = glm::vec4(255, 64, 64, 24) / 255.0f;

	glm::vec2 left = _unit->formation.GetFrontLeft(_unit->state.center);
	glm::vec2 right = left + _unit->formation.towardRight * (float)_unit->formation.numberOfFiles;
	glm::vec2 p;

	const float thickness = 4;
	const float radius_outer = 16;
	const float radius_inner = radius_outer - thickness;
	float radius_left = glm::distance(left, target);
	float radius_right = glm::distance(right, target);

	float angle_left = angle(left - target);
	float angle_right = angle(right - target);
	if (angle_left < angle_right)
		angle_left += 2 * glm::pi<float>();

	glm::vec2 delta = thickness * vector2_from_angle(angle_left + glm::half_pi<float>());
	renderer->AddVertex(GetPosition(left + delta), c0, true);
	renderer->AddVertex(GetPosition(left), c1);

	for (int i = 7; i >= 1; --i)
	{
		float r = i / 8.0f * radius_left;
		if (r > radius_outer)
		{
			p = target + r * vector2_from_angle(angle_left);
			renderer->AddVertex(GetPosition(p + delta), c0);
			renderer->AddVertex(GetPosition(p), c1);
		}
	}

	p = target + radius_outer * vector2_from_angle(angle_left);
	renderer->AddVertex(GetPosition(p + delta), c0);
	renderer->AddVertex(GetPosition(p), c1);

	p = target + radius_inner * vector2_from_angle(angle_left);
	renderer->AddVertex(GetPosition(p + delta), c0);
	renderer->AddVertex(GetPosition(p), c0);

	for (int i = 0; i <= 24; ++i)
	{
		float a = angle_left - i * (angle_left - angle_right) / 24;
		renderer->AddVertex(GetPosition(target + radius_outer * vector2_from_angle(a)), c1, i == 0);
		renderer->AddVertex(GetPosition(target + radius_inner * vector2_from_angle(a)), c0);
	}

	delta = thickness * vector2_from_angle(angle_right - glm::half_pi<float>());
	p = target + radius_inner * vector2_from_angle(angle_right);
	renderer->AddVertex(GetPosition(p + delta), c0);
	renderer->AddVertex(GetPosition(p + delta), c0);

	p = target + radius_outer * vector2_from_angle(angle_right);
	renderer->AddVertex(GetPosition(p), c1);
	renderer->AddVertex(GetPosition(p + delta), c0);

	for (int i = 1; i <= 7; ++i)
	{
		float r = i / 8.0f * radius_right;
		if (r > radius_outer)
		{
			p = target + r * vector2_from_angle(angle_right);
			renderer->AddVertex(GetPosition(p), c1);
			renderer->AddVertex(GetPosition(p + delta), c0);
		}
	}

	renderer->AddVertex(GetPosition(right), c1);
	renderer->AddVertex(GetPosition(right + delta), c0);
}



glm::vec3 RangeMarker::GetPosition(glm::vec2 p) const
{
	glm::vec3 result = _battleSimulator->GetHeightMap()->GetPosition(p, 1);
	if (result.z < 0.5f)
		result.z = 0.5f;
	return result;
}
