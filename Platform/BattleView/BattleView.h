// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEVIEW_H
#define BATTLEVIEW_H

#include "BattleMap/BattleMap.h"
#include "BattleModel/BattleObserver.h"
#include "BattleSimulator_v1_0_0.h"
#include "Shapes/VertexShape.h"
#include "Shapes/BillboardTextureShape.h"
#include "TerrainView/TerrainView.h"
#include "TiledTerrain/TiledTerrainRenderer.h"
#include "SmoothTerrain/SmoothTerrainRenderer.h"
#include "Graphics/CommonShaders.h"
#include "Surface/Animation.h"

class CasualtyMarker;
class UnitMovementMarker;
class RangeMarker;
class ShootingCounter;
class UnitTrackingMarker;
class UnitCounter;
class SmoothTerrainWater;
class SmoothTerrainSky;
class SmokeCounter;
class BattleHotspot;
class Touch;


class BattleView : public TerrainView, BattleObserver, BattleMapObserver, AnimationHost
{
	GraphicsContext* _gc{};
	BattleSimulator_v1_0_0* _simulator{};
	BattleObjects::Commander* _commander{};

	glm::vec3 _lightNormal{};

	BillboardTextureSheet* _billboardTexture{};
	BillboardModel* _billboardModel{};
	BillboardTextureShape* _textureBillboardShape{};
	BillboardTextureShape* _textureBillboardShape1{};
	BillboardTextureShape* _textureBillboardShape2{};

	CasualtyMarker* _casualtyMarker{};
	std::vector<UnitMovementMarker*> _movementMarkers{};
	std::vector<UnitTrackingMarker*> _trackingMarkers{};

	VertexShape_3f* _plainLineVertices{};
	VertexShape_3f_4f* _gradientLineVertices{};
	VertexShape_3f_4f* _gradientTriangleVertices{};
	VertexShape_3f_4f* _gradientTriangleStripVertices{};
	VertexShape_3f_4f_1f* _colorBillboardVertices{};
	VertexShape_3f_2f* _textureTriangleVertices{};
	VertexShape_2f_2f* _textureTriangleVertices2{};

	Texture* _textureUnitMarkers{};
	Texture* _textureTouchMarker{};

	std::vector<SmokeCounter*> _smokeMarkers{};
	std::vector<ShootingCounter*> _shootingCounters{};
	std::vector<UnitCounter*> _unitMarkers{};

	SmoothTerrainRenderer* _smoothTerrainSurface{};
	SmoothTerrainWater* _smoothTerrainWater{};
	SmoothTerrainSky* _smoothTerrainSky{};
	TiledTerrainRenderer* _tiledTerrainRenderer{};
	std::shared_ptr<BattleHotspot> _battleHotspot{};

public:
	BattleView(Surface* surface, std::shared_ptr<TerrainViewport> viewport);
	~BattleView();

	BattleSimulator_v1_0_0* GetSimulator() const { return _simulator; }
	void SetSimulator(BattleSimulator_v1_0_0* simulator);

	BattleObjects::Commander* GetCommander() const { return _commander; }
	void SetCommander(BattleObjects::Commander* value) { _commander = value; }

	SmoothTerrainRenderer* GetSmoothTerrainRenderer() const { return _smoothTerrainSurface; }
	SmoothTerrainWater* GetSmoothTerrainWater() const { return _smoothTerrainWater; }

private: // BattleObserver
	void OnAddUnit(BattleObjects::Unit* unit) override;
	void OnRemoveUnit(BattleObjects::Unit* unit) override;
	void OnCommand(BattleObjects::Unit* unit, float timer) override;
	void OnShooting(const BattleObjects_v1::Shooting& shooting, float timer) override;
	void OnRelease(const BattleObjects_v1::Shooting& shooting) override;
	void OnCasualty(const BattleObjects_v1::Fighter& fighter) override;
	void OnRouting(BattleObjects::Unit* unit) override;

private: // BattleMapObserver
	void OnBattleMapChanged(const BattleMap* battleMap) override;

public:
	void AddCasualty(const BattleObjects::Unit* unit, glm::vec2 position);

	const std::vector<UnitCounter*>& GetUnitCounters() const { return _unitMarkers; }

	UnitMovementMarker* AddMovementMarker(BattleObjects::Unit* unit);
	UnitMovementMarker* GetMovementMarker(BattleObjects::Unit* unit);
	UnitMovementMarker* GetNearestMovementMarker(glm::vec2 position, BattleObjects::Commander* commander);

	UnitTrackingMarker* AddTrackingMarker(BattleObjects::Unit* unit);
	UnitTrackingMarker* GetTrackingMarker(BattleObjects::Unit* unit);
	void RemoveTrackingMarker(UnitTrackingMarker* trackingMarker);

	void Initialize();
	void InitializeTerrainTrees();
	void UpdateTerrainTrees(bounds2f bounds);
	void InitializeCameraPosition();

public: // View
	void Render() override;
	void OnTouchBegin(Touch* touch) override;

private: // RenderLoopObserver
	void Animate(double secondsSinceLastUpdate) override;

public:
	bounds2f GetBillboardBounds(glm::vec3 position, float height);

	bounds2f GetUnitCurrentIconViewportBounds(BattleObjects::Unit* unit);
	bounds2f GetUnitFutureIconViewportBounds(BattleObjects::Unit* unit);

	bounds2f GetUnitFacingMarkerBounds(glm::vec2 center, float direction);
	bounds2f GetUnitCurrentFacingMarkerBounds(BattleObjects::Unit* unit);
	bounds2f GetUnitFutureFacingMarkerBounds(BattleObjects::Unit* unit);

	bounds1f GetUnitIconSizeLimit() const;

	/***/

	void AddShootingAndSmokeCounters(const BattleObjects_v1::Shooting& shooting);

	void AddShootingCounter(const BattleObjects_v1::Shooting& shooting);
	ShootingCounter* AddShootingCounter(BattleObjects_v1::MissileType missileType);
	void RemoveAllShootingMarkers();

	void AddSmokeMarker(const BattleObjects_v1::Shooting& shooting);
	SmokeCounter* AddSmokeMarker(BattleObjects_v1::MissileType missileType);
	void RemoveAllSmokeMarkers();

	UnitCounter* GetNearestUnitCounter(glm::vec2 position, int filterTeam, BattleObjects::Commander* filterCommander, bool filterDeployed);

	void UpdateSoundPlayer();
	void UpdateDeploymentZones();
};


#endif
