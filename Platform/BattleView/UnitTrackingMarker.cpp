// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/constants.hpp>

#include "BattleModel/BattleCommander.h"
#include "BillboardTextureShape.h"
#include "Shapes/PathRenderer.h"
#include "BillboardColorShader.h"
#include "BattleView.h"
#include "UnitTrackingMarker.h"


UnitTrackingMarker::UnitTrackingMarker(BattleView* battleView, Unit* unit) : UnitMarker(battleView, unit),
_meleeTarget(0),
_destination(_unit->state.center),
_hasDestination(false),
_missileTarget(0),
_orientation(),
_hasOrientation(false),
_renderOrientation(false),
_running(false)
{

}


UnitTrackingMarker::~UnitTrackingMarker()
{
}


float UnitTrackingMarker::GetFacing() const
{
	glm::vec2 orientation = _missileTarget ? _missileTarget->state.center : _orientation;
	return angle(orientation - DestinationXXX());
}


void UnitTrackingMarker::RenderTrackingFighters(VertexShape_3f_4f_1f* vertices)
{
	if (!_meleeTarget && !_missileTarget)
	{
		bool isBlue = _unit->commander->GetTeam() == _battleView->GetCommander()->GetTeam();
		glm::vec4 color = isBlue ? glm::vec4(0, 0, 255, 16) / 255.0f : glm::vec4(255, 0, 0, 16) / 255.0f;

		glm::vec2 destination = DestinationXXX();
		//glm::vec2 orientation = _missileTarget ? _missileTarget->state.center : _orientation;

		Formation formation = _unit->formation;
		formation.SetDirection(GetFacing());

		glm::vec2 frontLeft = formation.GetFrontLeft(destination);

		for (Fighter* fighter = _unit->fighters, * end = fighter + _unit->fightersCount; fighter != end; ++fighter)
		{
			glm::vec2 offsetRight = formation.towardRight * (float)Unit::GetFighterFile(fighter);
			glm::vec2 offsetBack = formation.towardBack * (float)Unit::GetFighterRank(fighter);
			glm::vec3 p = _battleView->GetSimulator()->GetHeightMap()->GetPosition(frontLeft + offsetRight + offsetBack, 0.5);
			vertices->AddVertex(Vertex_3f_4f_1f(p, color, 3.0));
		}
	}
}



void UnitTrackingMarker::RenderTrackingMarker(BillboardTextureShape* renderer)
{
	if (_meleeTarget == nullptr)
	{
		glm::vec2 destination = DestinationXXX();
		glm::vec3 position = _battleView->GetSimulator()->GetHeightMap()->GetPosition(destination, 0);
		glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
		glm::vec2 texcoord = texsize * glm::vec2(_unit->commander->GetTeam() == _battleView->GetCommander()->GetTeam() ? 2 : 0, 2);

		renderer->AddBillboard(position, 32, affine2(texcoord, texcoord + texsize));
	}
}


void UnitTrackingMarker::AppendFacingMarker(VertexShape_3f_2f* vertices, BattleView* battleView)
{
	if (_path.empty())
		return;

	float facing = GetFacing();

	bounds2f b = battleView->GetUnitFacingMarkerBounds(_path.back(), facing);
	glm::vec2 p = b.center();
	float size = b.height();
	float direction = facing - battleView->GetCameraFacing();
	if (battleView->GetFlip())
		direction += glm::pi<float>();

	glm::vec2 d1 = size * vector2_from_angle(direction - glm::half_pi<float>() / 2.0f);
	glm::vec2 d2 = glm::vec2(d1.y, -d1.x);
	glm::vec2 d3 = glm::vec2(d2.y, -d2.x);
	glm::vec2 d4 = glm::vec2(d3.y, -d3.x);

	float tx1 = 0.125f;
	float tx2 = 0.125f + 0.125f;

	float ty1 = 0.75f;
	float ty2 = 0.75f + 0.125f;

	vertices->AddVertex(Vertex_3f_2f(glm::vec3(p + d1, 0), glm::vec2(tx1, ty1)));
	vertices->AddVertex(Vertex_3f_2f(glm::vec3(p + d2, 0), glm::vec2(tx1, ty2)));
	vertices->AddVertex(Vertex_3f_2f(glm::vec3(p + d3, 0), glm::vec2(tx2, ty2)));

	vertices->AddVertex(Vertex_3f_2f(glm::vec3(p + d3, 0), glm::vec2(tx2, ty2)));
	vertices->AddVertex(Vertex_3f_2f(glm::vec3(p + d4, 0), glm::vec2(tx2, ty1)));
	vertices->AddVertex(Vertex_3f_2f(glm::vec3(p + d1, 0), glm::vec2(tx1, ty1)));
}


void UnitTrackingMarker::RenderTrackingShadow(BillboardTextureShape* renderer)
{
	glm::vec2 destination = DestinationXXX();
	glm::vec3 position = _battleView->GetSimulator()->GetHeightMap()->GetPosition(destination, 0);

	renderer->AddBillboard(position, 32, affine2(glm::vec2(0, 0), glm::vec2(1, 1)));
}



void UnitTrackingMarker::RenderTrackingPath(VertexShape_3f_4f* vertices)
{
	if (!_path.empty())
	{
		int mode = 0;
		if (_meleeTarget)
			mode = 2;
		else if (_running)
			mode = 1;

		HeightMap* heightMap = _battleView->GetSimulator()->GetHeightMap();
		PathRenderer pathRenderer([heightMap](glm::vec2 p) { return heightMap->GetPosition(p, 1); });
		pathRenderer.Path(vertices, _path, mode);
	}
}


void UnitTrackingMarker::RenderOrientation(VertexShape_3f_4f* vertices)
{
	if (_renderOrientation && _hasOrientation && !_path.empty())
	{
		glm::vec2 tip = _missileTarget != nullptr ? _missileTarget->state.center : _orientation;
		float overshoot = _missileTarget != nullptr ? 5 : 20;
		glm::vec2 center = _path.back();
		glm::vec2 diff = tip - center;
		if (glm::length(diff) < 0.1)
			return;

		glm::vec2 dir = glm::normalize(diff);
		glm::vec2 left = glm::vec2(dir.y, -dir.x);


		vertices->AddVertex(Vertex_3f_4f(_battleView->GetSimulator()->GetHeightMap()->GetPosition(center + 10.0f * left, 0), glm::vec4(0, 0, 0, 0)));
		vertices->AddVertex(Vertex_3f_4f(_battleView->GetSimulator()->GetHeightMap()->GetPosition(tip + overshoot * dir, 0), glm::vec4(0, 0, 0, 0.1f)));
		vertices->AddVertex(Vertex_3f_4f(_battleView->GetSimulator()->GetHeightMap()->GetPosition(center - 10.0f * left, 0), glm::vec4(0, 0, 0, 0)));
	}
}
