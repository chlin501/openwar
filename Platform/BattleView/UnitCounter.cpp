// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/constants.hpp>

#include "Shapes/BillboardTextureShape.h"
#include "BattleMap/BattleMap.h"
#include "BattleSimulator_v1_0_0.h"
#include "BattleView.h"
#include "UnitCounter.h"
#include "TerrainView/TerrainViewport.h"
#include "BattleScenario.h"


UnitCounter::UnitCounter(BattleView* battleView, BattleObjects::Unit* unit) :
_battleView{battleView},
_unit{unit}
{
	_samuraiWeapon = BattleObjects_v1::GetSamuraiWeapon(unit->unitClass.c_str());
	_samuraiPlatform = BattleObjects_v1::GetSamuraiPlatform(unit->unitClass.c_str());
}


UnitCounter::~UnitCounter()
{
}


bool UnitCounter::Animate(float seconds)
{
	float routingBlinkTime = GetRoutingBlinkTime(_unit);

	if (!_unit->IsRouting() && routingBlinkTime != 0)
	{
		_routingTimer -= seconds;
		if (_routingTimer < 0)
			_routingTimer = routingBlinkTime;
	}

	return true;
}


void UnitCounter::AppendUnitMarker(BillboardTextureShape* renderer, bool flip)
{
	bool routingIndicator = false;
	float routingBlinkTime = GetRoutingBlinkTime(_unit);

	if (_unit->IsRouting())
	{
		routingIndicator = true;
	}
	else if (routingBlinkTime != 0 && 0 <= _routingTimer && _routingTimer < 0.2f)
	{
		routingIndicator = true;
	}

	int color = 3;
	if (!routingIndicator)
	{
		if (_unit->GetTeam() != _battleView->GetCommander()->GetTeam())
			color = 0;
		else if (_battleView->GetBattleScenario()->GetCommanderForUnit(_unit) != _battleView->GetCommander())
			color = 1;
		else
			color = 2;
	}

	int command = !_unit->deployed ? 2
		: !_battleView->GetBattleScenario()->IsCommandableBy(_unit, _battleView->GetCommander()) ? 1
		: 0;

	glm::vec3 position = _battleView->GetBattleSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(_unit->GetCenter(), 0);
	glm::vec2 texsize(0.1875f, 0.1875f); // 48 / 256
	glm::vec2 texcoord1 = texsize * glm::vec2(color, command);
	glm::vec2 texcoord2 = texsize * glm::vec2((int)_samuraiPlatform, 3);
	glm::vec2 texcoord3 = texsize * glm::vec2(4, (int)_samuraiWeapon);

	if (flip)
	{
		texcoord1 += texsize;
		texcoord2 += texsize;
		texcoord3 += texsize;
		texsize = -texsize;
	}

	renderer->AddBillboard(position, 32, affine2(texcoord1, texcoord1 + texsize));
	renderer->AddBillboard(position, 32, affine2(texcoord2, texcoord2 + texsize));
	renderer->AddBillboard(position, 32, affine2(texcoord3, texcoord3 + texsize));
}


void UnitCounter::AppendFacingMarker(VertexShape_2f_2f* vertices, BattleView* battleView)
{
	if (!_battleView->GetBattleScenario()->IsCommandableBy(_unit, _battleView->GetCommander()))
		return;

	const BattleObjects::UnitCommand& command = _unit->GetIssuedCommand();

	if (!_unit->IsStanding() || command.meleeTarget || _unit->IsRouting())
	{
		return;
	}

	int xindex = 0;
	if (!_unit->deployed)
	{
		xindex = 1;
	}
	else if (command.missileTarget == _unit)
	{
		xindex = 11;
	}
	else
	{
		std::pair<bool, float> loadingProgress = _unit->GetLoadingProgress();
		if (loadingProgress.first)
		{
			xindex = 2 + (int)glm::round(9.0f * loadingProgress.second);
			xindex = glm::min(10, xindex);
		}
	}
	int yindex = 0;
	if (xindex >= 6)
	{
		xindex -= 6;
		yindex += 1;
	}

	float tx1 = xindex * 0.125f;
	float tx2 = tx1 + 0.125f;

	float ty1 = 0.75f + yindex * 0.125f;
	float ty2 = ty1 + 0.125f;

	TerrainViewport* terrainViewport = &battleView->GetTerrainViewport();

	bounds2f bounds = battleView->GetUnitCurrentFacingMarkerBounds(_unit);
	glm::vec2 p = bounds.mid();
	float size = bounds.y().size();
	float direction = xindex >= 2 || yindex != 0 ? -glm::half_pi<float>() : (_unit->GetBearing() - terrainViewport->GetCameraFacing());
	if (terrainViewport->GetFlip())
		direction += glm::pi<float>();

	glm::vec2 d1 = size * vector2_from_angle(direction - glm::half_pi<float>() / 2.0f);
	glm::vec2 d2 = glm::vec2(d1.y, -d1.x);
	glm::vec2 d3 = glm::vec2(d2.y, -d2.x);
	glm::vec2 d4 = glm::vec2(d3.y, -d3.x);

	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d1), glm::vec2(tx1, ty2)));
	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d2), glm::vec2(tx1, ty1)));
	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d3), glm::vec2(tx2, ty1)));
	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d3), glm::vec2(tx2, ty1)));
	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d4), glm::vec2(tx2, ty2)));
	vertices->AddVertex(Vertex_2f_2f(terrainViewport->LocalToNormalized(p + d1), glm::vec2(tx1, ty2)));
}


void UnitCounter::AppendFighterWeapons(VertexShape_3f* vertices)
{
	float weaponReach = _unit->GetWeaponReach();
	if (weaponReach > 0)
	{
		int count = _unit->GetFighterCount();
		for (int index = 0; index < count; ++index)
		{
			BattleObjects::FighterPosition fighter = _unit->GetFighterPosition(index);
			glm::vec2 p1 = fighter.position;
			glm::vec2 p2 = p1 + weaponReach * vector2_from_angle(fighter.bearing);

			vertices->AddVertex(Vertex_3f(_battleView->GetBattleSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(p1, 1)));
			vertices->AddVertex(Vertex_3f(_battleView->GetBattleSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(p2, 1)));
		}
	}
}


void UnitCounter::AppendFighterBillboards(BillboardModel* billboardModel)
{
	bool isSameTeam = _unit->GetTeam() == _battleView->GetCommander()->GetTeam();
	float size = 2.0;
	int shape = 0;
	switch (_samuraiPlatform)
	{
		case BattleObjects_v1::SamuraiPlatform_Cav:
		case BattleObjects_v1::SamuraiPlatform_Gen:
			shape = isSameTeam ? billboardModel->_billboardShapeFighterCavBlue : billboardModel->_billboardShapeFighterCavRed;
			size = 3.0;
			break;

		case BattleObjects_v1::SamuraiPlatform_Sam:
			shape = isSameTeam ? billboardModel->_billboardShapeFighterSamBlue : billboardModel->_billboardShapeFighterSamRed;
			size = 2.0;
			break;

		case BattleObjects_v1::SamuraiPlatform_Ash:
			shape = isSameTeam ? billboardModel->_billboardShapeFighterAshBlue : billboardModel->_billboardShapeFighterAshRed;
			size = 2.0;
			break;
	}

	int count = _unit->GetFighterCount();
	for (int index = 0; index < count; ++index)
	{
		BattleObjects::FighterPosition fighter = _unit->GetFighterPosition(index);
		const float adjust = 0.5f - 2.0f / 64.0f; // place texture 2 texels below ground
		glm::vec3 p = _battleView->GetBattleSimulator()->GetBattleMap()->GetHeightMap()->GetPosition(fighter.position, adjust * size);
		float facing = glm::degrees(fighter.bearing);
		billboardModel->dynamicBillboards.push_back(Billboard(p, facing, size, shape));
	}
}


float UnitCounter::GetRoutingBlinkTime(BattleObjects::Unit* unit)
{
	float morale = unit->GetEffectiveMorale();
	return 0 <= morale && morale < 0.33f ? 0.1f + morale * 3 : 0;
}
