// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/constants.hpp>

#include "BattleMap/BattleMap.h"
#include "Shapes/BillboardTextureShape.h"
#include "Shapes/PathRenderer.h"
#include "Shapes/BillboardColorShader.h"
#include "BattleView.h"
#include "UnitTrackingMarker.h"
#include "TerrainView/TerrainViewport.h"


UnitTrackingMarker::UnitTrackingMarker(BattleView* battleView, BattleObjects::Unit* unit) : UnitMarker(battleView, unit),
_destination{_unit->GetCenter()}
{
}


UnitTrackingMarker::~UnitTrackingMarker()
{
}


float UnitTrackingMarker::GetFacing() const
{
	glm::vec2 orientation = _missileTarget ? _missileTarget->GetCenter() : _orientation;
	return angle(orientation - DestinationXXX());
}


void UnitTrackingMarker::RenderTrackingFighters(VertexShape_3f_4f_1f* vertices)
{
	if (!_meleeTarget && !_missileTarget)
	{
		bool isBlue = _unit->GetTeam() == _battleView->GetCommander()->GetTeam();
		glm::vec4 color = isBlue ? glm::vec4(0, 0, 255, 16) / 255.0f : glm::vec4(255, 0, 0, 16) / 255.0f;

		glm::vec2 destination = DestinationXXX();
		//glm::vec2 orientation = _missileTarget ? _missileTarget->state.center : _orientation;

		BattleObjects::Formation formation = _unit->GetFormation();
		formation.SetDirection(GetFacing());

		glm::vec2 frontLeft = formation.GetFrontLeft(destination);

		int count = _unit->GetFighterCount();
		for (int index = 0; index < count; ++index)
		{
			BattleObjects::FighterAssignment assignment = _unit->GetFighterAssignment(index);
			glm::vec2 offsetRight = formation.towardRight * (float)assignment.file;
			glm::vec2 offsetBack = formation.towardBack * (float)assignment.rank;
			glm::vec3 p = _battleView->GetBattleSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(frontLeft + offsetRight + offsetBack, 0.5);
			vertices->AddVertex(Vertex_3f_4f_1f(p, color, 3.0));
		}
	}
}



void UnitTrackingMarker::RenderTrackingMarker(BillboardTextureShape* renderer)
{
	if (_meleeTarget == nullptr)
	{
		glm::vec2 destination = DestinationXXX();
		glm::vec3 position = _battleView->GetBattleSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(destination, 0);
		glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
		glm::vec2 texcoord = texsize * glm::vec2(_unit->GetTeam() == _battleView->GetCommander()->GetTeam() ? 2 : 0, 2);

		renderer->AddBillboard(position, 32, affine2(texcoord, texcoord + texsize));
	}
}


void UnitTrackingMarker::AppendFacingMarker(VertexShape_2f_2f* vertices, BattleView* battleView)
{
	if (_path.empty())
		return;

	TerrainViewport* terrainViewport = &battleView->GetTerrainViewport();

	float facing = GetFacing();

	bounds2f b = battleView->GetUnitFacingMarkerBounds(_path.back(), facing);
	glm::vec2 p = b.mid();
	float size = b.y().size();
	float direction = facing - terrainViewport->GetCameraFacing();
	if (terrainViewport->GetFlip())
		direction += glm::pi<float>();

	glm::vec2 d1 = size * vector2_from_angle(direction - glm::half_pi<float>() / 2.0f);
	glm::vec2 d2 = glm::vec2(d1.y, -d1.x);
	glm::vec2 d3 = glm::vec2(d2.y, -d2.x);
	glm::vec2 d4 = glm::vec2(d3.y, -d3.x);

	float tx1 = 0.125f;
	float tx2 = 0.125f + 0.125f;

	float ty1 = 0.75f;
	float ty2 = 0.75f + 0.125f;

	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d1), glm::vec2(tx1, ty1)));
	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d2), glm::vec2(tx1, ty2)));
	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d3), glm::vec2(tx2, ty2)));
	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d3), glm::vec2(tx2, ty2)));
	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d4), glm::vec2(tx2, ty1)));
	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d1), glm::vec2(tx1, ty1)));
}


void UnitTrackingMarker::RenderTrackingShadow(BillboardTextureShape* renderer)
{
	glm::vec2 destination = DestinationXXX();
	glm::vec3 position = _battleView->GetBattleSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(destination, 0);

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

		const HeightMap* heightMap = _battleView->GetBattleSimulator()->GetBattleMap()->GetHeightMap();
		PathRenderer pathRenderer([heightMap](glm::vec2 p) { return heightMap->GetPosition(p, 1); });
		pathRenderer.Path(vertices, _path, mode);
	}
}


void UnitTrackingMarker::RenderOrientation(VertexShape_3f_4f* vertices)
{
	if (_renderOrientation && _hasOrientation && !_path.empty())
	{
		glm::vec2 tip = _missileTarget ? _missileTarget->GetCenter() : _orientation;
		float overshoot = _missileTarget ? 5 : 20;
		glm::vec2 center = _path.back();
		glm::vec2 diff = tip - center;
		if (glm::length(diff) < 0.1)
			return;

		glm::vec2 dir = glm::normalize(diff);
		glm::vec2 left = glm::vec2(dir.y, -dir.x);


		vertices->AddVertex(Vertex_3f_4f(_battleView->GetBattleSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(center + 10.0f * left, 0), glm::vec4(0, 0, 0, 0)));
		vertices->AddVertex(Vertex_3f_4f(_battleView->GetBattleSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(tip + overshoot * dir, 0), glm::vec4(0, 0, 0, 0.1f)));
		vertices->AddVertex(Vertex_3f_4f(_battleView->GetBattleSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(center - 10.0f * left, 0), glm::vec4(0, 0, 0, 0)));
	}
}
