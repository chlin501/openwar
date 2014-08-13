// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEVIEW_H
#define BATTLEVIEW_H

#include "BattleModel/BattleSimulator.h"
#include "Graphics/vertexbuffer.h"
#include "Shapes/TextureBillboardRenderer.h"
#include "TerrainView/TerrainView.h"
#include "TiledTerrain/TiledTerrainRenderer.h"
#include "SmoothTerrain/SmoothTerrainRenderer.h"

class BattleCommander;
class CasualtyMarker;
class ColorBillboardShape;
class GradientLineShape3;
class GradientTriangleShape3;
class GradientTriangleStripShape3;
class UnitMovementMarker;
class PlainLineShape3;
class PlainTriangleShape3;
class RangeMarker;
class ShootingCounter;
class TextureTriangleShape3;
class UnitTrackingMarker;
class UnitCounter;
class SmoothTerrainWater;
class SmoothTerrainSky;
class SmokeCounter;


class BattleView : public TerrainView, public BattleObserver
{
	BattleSimulator* _simulator;
	BattleCommander* _commander;

	renderers* _renderers;

	glm::vec3 _lightNormal;

	BillboardTexture* _billboardTexture;
	BillboardModel* _billboardModel;
	TextureBillboardRenderer* _textureBillboardRenderer;
	TextureBillboardRenderer* _textureBillboardRenderer1;
	TextureBillboardRenderer* _textureBillboardRenderer2;

	CasualtyMarker* _casualtyMarker;
	std::vector<UnitMovementMarker*> _movementMarkers;
	std::vector<UnitTrackingMarker*> _trackingMarkers;

	PlainLineShape3* _plainLineRenderer;
	PlainTriangleShape3* _plainTriangleRenderer;
	GradientLineShape3* _gradientLineRenderer;
	GradientTriangleShape3* _gradientTriangleRenderer;
	GradientTriangleStripShape3* _gradientTriangleStripRenderer;
	ColorBillboardShape* _colorBillboardRenderer;
	TextureTriangleShape3* _textureTriangleRenderer;

	texture* _textureUnitMarkers;
	texture* _textureTouchMarker;

	std::vector<SmokeCounter*> _smokeMarkers;
	std::vector<ShootingCounter*> _shootingCounters;
	std::vector<UnitCounter*> _unitMarkers;

	SmoothTerrainRenderer* _smoothTerrainSurface;
	SmoothTerrainWater* _smoothTerrainWater;
	SmoothTerrainSky* _smoothTerrainSky;
	TiledTerrainRenderer* _tiledTerrainRenderer;

	bool _initializeCameraPositionOnAddUnit;

public:
	BattleView(graphicscontext* gc, renderers* r);
	~BattleView();

	BattleSimulator* GetSimulator() const { return _simulator; }
	void SetSimulator(BattleSimulator* simulator);

	BattleCommander* GetCommander() const { return _commander; }
	void SetCommander(BattleCommander* value) { _commander = value; }

	SmoothTerrainRenderer* GetSmoothTerrainRenderer() const { return _smoothTerrainSurface; }
	SmoothTerrainWater* GetSmoothTerrainWater() const { return _smoothTerrainWater; }

	// BattleObserver
	virtual void OnSetGroundMap(GroundMap* groundMap);
	virtual void OnAddUnit(Unit* unit);
	virtual void OnRemoveUnit(Unit* unit);
	virtual void OnCommand(Unit* unit, float timer);
	virtual void OnShooting(const Shooting& shooting, float timer);
	virtual void OnRelease(const Shooting& shooting);
	virtual void OnCasualty(const Fighter& fighter);
	virtual void OnRouting(Unit* unit);

	void AddCasualty(Unit* unit, glm::vec2 position);

	const std::vector<UnitCounter*>& GetUnitCounters() const { return _unitMarkers; }

	UnitMovementMarker* AddMovementMarker(Unit* unit);
	UnitMovementMarker* GetMovementMarker(Unit* unit);
	UnitMovementMarker* GetNearestMovementMarker(glm::vec2 position, BattleCommander* commander);

	UnitTrackingMarker* AddTrackingMarker(Unit* unit);
	UnitTrackingMarker* GetTrackingMarker(Unit* unit);
	void RemoveTrackingMarker(UnitTrackingMarker* trackingMarker);

	void Initialize();

	void InitializeTerrainTrees();
	void UpdateTerrainTrees(bounds2f bounds);

	void InitializeCameraPosition();

	virtual void Render(const glm::mat4& transform);
	virtual void Update(double secondsSinceLastUpdate);
	virtual void FindHotspots(const glm::mat4 transform, glm::vec2 position, std::function<void(std::shared_ptr<Hotspot>)> action);

	bounds2f GetBillboardBounds(glm::vec3 position, float height);

	bounds2f GetUnitCurrentIconViewportBounds(Unit* unit);
	bounds2f GetUnitFutureIconViewportBounds(Unit* unit);

	bounds2f GetUnitFacingMarkerBounds(glm::vec2 center, float direction);
	bounds2f GetUnitCurrentFacingMarkerBounds(Unit* unit);
	bounds2f GetUnitFutureFacingMarkerBounds(Unit* unit);

	bounds1f GetUnitIconSizeLimit() const;

	/***/

	void AnimateMarkers(float seconds);

	void AddShootingAndSmokeCounters(const Shooting& shooting);

	void AddShootingCounter(const Shooting& shooting);
	ShootingCounter* AddShootingCounter(MissileType missileType);
	void RemoveAllShootingMarkers();

	void AddSmokeMarker(const Shooting& shooting);
	SmokeCounter* AddSmokeMarker(MissileType missileType);
	void RemoveAllSmokeMarkers();

	UnitCounter* GetNearestUnitCounter(glm::vec2 position, int team, BattleCommander* commander);
};


#endif
