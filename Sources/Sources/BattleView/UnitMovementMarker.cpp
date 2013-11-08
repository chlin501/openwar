// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/constants.hpp>

#include "../../BattleModel/BattleSimulator.h"
#include "../../Library/Renderers/ColorBillboardRenderer.h"
#include "../../Library/Renderers/PathRenderer.h"
#include "../../Library/Renderers/TextureBillboardRenderer.h"
#include "../../Library/Renderers/TextureRenderer.h"
#include "BattleView.h"
#include "UnitMovementMarker.h"



UnitMovementMarker::UnitMovementMarker(BattleView* battleView, Unit* unit) : UnitMarker(battleView, unit)
{
}


UnitMovementMarker::~UnitMovementMarker()
{
}


bool UnitMovementMarker::Animate(float seconds)
{
	return !_unit->state.IsRouting()
		&& MovementRules::Length(_unit->command.path) > 8;
}


void UnitMovementMarker::RenderMovementMarker(TextureBillboardRenderer* renderer)
{
	glm::vec2 finalDestination = _unit->command.GetDestination();
	if (_unit->command.path.size() > 1 || glm::length(_unit->state.center - finalDestination) > 25)
	{
		if (_unit->command.meleeTarget == nullptr)
		{
			glm::vec3 position = _battleView->GetBattleSimulator()->GetHeightMap()->GetPosition(finalDestination, 0.5);
			glm::vec2 texsize(0.1875, 0.1875); // 48 / 256
			glm::vec2 texcoord = texsize * glm::vec2(_unit->team != _battleView->_blueTeam ? 4 : 3, 0);

			renderer->AddBillboard(position, 32, affine2(texcoord, texcoord + texsize));
		}
	}
}


void UnitMovementMarker::AppendFacingMarker(TextureTriangleRenderer* renderer, BattleView* battleView)
{
	if (_unit->state.unitMode != UnitMode_Moving)
		return;

	bounds2f b = battleView->GetUnitFutureFacingMarkerBounds(_unit);
	glm::vec2 p = b.center();
	float size = b.height();
	float direction = _unit->command.bearing - battleView->GetCameraFacing();
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

	renderer->AddVertex(glm::vec3(p + d1, 0), glm::vec2(tx1, ty1));
	renderer->AddVertex(glm::vec3(p + d2, 0), glm::vec2(tx1, ty2));
	renderer->AddVertex(glm::vec3(p + d3, 0), glm::vec2(tx2, ty2));

	renderer->AddVertex(glm::vec3(p + d3, 0), glm::vec2(tx2, ty2));
	renderer->AddVertex(glm::vec3(p + d4, 0), glm::vec2(tx2, ty1));
	renderer->AddVertex(glm::vec3(p + d1, 0), glm::vec2(tx1, ty1));
}


void UnitMovementMarker::RenderMovementFighters(ColorBillboardRenderer* renderer)
{
	if (_unit->command.meleeTarget == nullptr)
	{
		bool isBlue = _unit->team == _battleView->_blueTeam;
		glm::vec4 color = isBlue ? glm::vec4(0, 0, 255, 32) / 255.0f : glm::vec4(255, 0, 0, 32) / 255.0f;

		glm::vec2 finalDestination = _unit->command.GetDestination();

		Formation formation = _unit->formation;
		formation.SetDirection(_unit->command.bearing);

		glm::vec2 frontLeft = formation.GetFrontLeft(finalDestination);

		for (Fighter* fighter = _unit->fighters, * end = fighter + _unit->fightersCount; fighter != end; ++fighter)
		{
			glm::vec2 offsetRight = formation.towardRight * (float)Unit::GetFighterFile(fighter);
			glm::vec2 offsetBack = formation.towardBack * (float)Unit::GetFighterRank(fighter);

			renderer->AddBillboard(_battleView->GetBattleSimulator()->GetHeightMap()->GetPosition(frontLeft + offsetRight + offsetBack, 0.5), color, 3.0);
		}
	}
}


void UnitMovementMarker::RenderMovementPath(GradientTriangleRenderer* renderer)
{
	if (!_unit->command.path.empty())
	{
		int mode = 0;
		if (_unit->command.meleeTarget != nullptr)
			mode = 2;
		else if (_unit->command.running)
			mode = 1;

		HeightMap* heightMap = _battleView->GetBattleSimulator()->GetHeightMap();
		PathRenderer pathRenderer([heightMap](glm::vec2 p) { return heightMap->GetPosition(p, 1); });
		pathRenderer.Path(renderer, _unit->command.path, mode);
	}
}
