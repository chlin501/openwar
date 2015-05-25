// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/constants.hpp>

#include "BattleMap/BattleMap.h"
#include "BattleModel/BattleSimulator.h"
#include "BattleModel/BattleCommander.h"
#include "Shapes/BillboardColorShader.h"
#include "Shapes/PathRenderer.h"
#include "Shapes/BillboardTextureShape.h"
#include "BattleView.h"
#include "UnitMovementMarker.h"
#include "TerrainView/TerrainViewport.h"


UnitMovementMarker::UnitMovementMarker(BattleView* battleView, Unit* unit) : UnitMarker(battleView, unit)
{
}


UnitMovementMarker::~UnitMovementMarker()
{
}


bool UnitMovementMarker::Animate(float seconds)
{
	if (_unit->state.IsRouting())
		return false;

	const UnitCommand& command = _unit->GetCommand();
	return command.path.size() > 2 || MovementRules::Length(command.path) > 8;
}


void UnitMovementMarker::RenderMovementMarker(BillboardTextureShape* renderer)
{
	if (!_unit->IsCommandableBy(_battleView->GetCommander()))
		return;

	const UnitCommand& command = _unit->GetCommand();
	glm::vec2 finalDestination = command.GetDestination();
	if (command.path.size() > 1 || glm::length(_unit->state.center - finalDestination) > 25)
	{
		if (command.meleeTarget == nullptr)
		{
			glm::vec3 position = _battleView->GetSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(finalDestination, 0.5);
			glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
			glm::vec2 texcoord = texsize * glm::vec2(_unit->commander->GetTeam() == _battleView->GetCommander()->GetTeam() ? 2 : 0, 2);

			renderer->AddBillboard(position, 32, affine2(texcoord, texcoord + texsize));
		}
	}
}


void UnitMovementMarker::AppendFacingMarker(VertexShape_2f_2f* vertices, BattleView* battleView)
{
	if (!_unit->IsCommandableBy(_battleView->GetCommander()))
		return;
	if (_unit->state.unitMode != UnitMode_Moving)
		return;

	const UnitCommand& command = _unit->GetCommand();

	TerrainViewport* terrainViewport = battleView->GetTerrainViewport();

	bounds2f b = battleView->GetUnitFutureFacingMarkerBounds(_unit);
	glm::vec2 p = b.mid();
	float size = b.y().size();
	float direction = command.bearing - terrainViewport->GetCameraFacing();
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


void UnitMovementMarker::RenderMovementFighters(VertexShape_3f_4f_1f* vertices)
{
	if (!_unit->IsCommandableBy(_battleView->GetCommander()))
		return;

	const UnitCommand& command = _unit->GetCommand();
	if (command.meleeTarget == nullptr)
	{
		bool isBlue = _unit->commander->GetTeam() == _battleView->GetCommander()->GetTeam();
		glm::vec4 color = isBlue ? glm::vec4(0, 0, 255, 32) / 255.0f : glm::vec4(255, 0, 0, 32) / 255.0f;

		glm::vec2 finalDestination = command.GetDestination();

		Formation formation = _unit->formation;
		formation.SetDirection(command.bearing);

		glm::vec2 frontLeft = formation.GetFrontLeft(finalDestination);

		for (Fighter* fighter = _unit->fighters, * end = fighter + _unit->fightersCount; fighter != end; ++fighter)
		{
			glm::vec2 offsetRight = formation.towardRight * (float)Unit::GetFighterFile(fighter);
			glm::vec2 offsetBack = formation.towardBack * (float)Unit::GetFighterRank(fighter);
			glm::vec3 p = _battleView->GetSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(frontLeft + offsetRight + offsetBack, 0.5);
			vertices->AddVertex(Vertex_3f_4f_1f(p, color, 3.0));
		}
	}
}


void UnitMovementMarker::RenderMovementPath(VertexShape_3f_4f* vertices)
{
	const UnitCommand& command = _unit->GetCommand();
	if (!command.path.empty())
	{
		int mode = 0;
		if (command.meleeTarget)
			mode = 2;
		else if (command.running)
			mode = 1;

		const HeightMap* heightMap = _battleView->GetSimulator()->GetBattleMap()->GetHeightMap();
		PathRenderer pathRenderer([heightMap](glm::vec2 p) { return heightMap->GetPosition(p, 1); });
		pathRenderer.Path(vertices, command.path, mode);
	}
}
