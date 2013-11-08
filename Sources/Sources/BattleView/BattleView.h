// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEVIEW_H
#define BATTLEVIEW_H

#include "../../BattleModel/BattleSimulator.h"
#include "../../Library/Graphics/vertexbuffer.h"
#include "../../Library/Renderers/TextureBillboardRenderer.h"
#include "../TerrainView/TerrainView.h"
#include "../TiledTerrain/TiledTerrainRenderer.h"
#include "../SmoothTerrain/SmoothTerrainRenderer.h"

class BattleCommander;
class CasualtyMarker;
class ColorBillboardRenderer;
class GradientLineRenderer;
class GradientTriangleRenderer;
class GradientTriangleStripRenderer;
class UnitMovementMarker;
class PlainLineRenderer;
class PlainTriangleRenderer;
class RangeMarker;
class ShootingCounter;
class TextureTriangleRenderer;
class UnitTrackingMarker;
class UnitCounter;
class SmoothTerrainWater;
class SmoothTerrainSky;
class SmokeCounter;


class BattleView : public TerrainView, public BattleObserver
{
	BattleSimulator* _battleSimulator;
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

	PlainLineRenderer* _plainLineRenderer;
	PlainTriangleRenderer* _plainTriangleRenderer;
	GradientLineRenderer* _gradientLineRenderer;
	GradientTriangleRenderer* _gradientTriangleRenderer;
	GradientTriangleStripRenderer* _gradientTriangleStripRenderer;
	ColorBillboardRenderer* _colorBillboardRenderer;
	TextureTriangleRenderer* _textureTriangleRenderer;

	texture* _textureUnitMarkers;
	texture* _textureTouchMarker;

	std::vector<SmokeCounter*> _smokeMarkers;
	std::vector<ShootingCounter*> _shootingCounters;
	std::vector<UnitCounter*> _unitMarkers;

	BattleCommander* _commander;

public:
	SmoothTerrainRenderer* _smoothTerrainSurface;
	SmoothTerrainWater* _smoothTerrainWater;
	SmoothTerrainSky* _smoothTerrainSky;
	TiledTerrainRenderer* _tiledTerrainRenderer;

public:
	BattleView(Surface* screen, BattleSimulator* battleSimulator, renderers* r);
	~BattleView();

	BattleSimulator* GetBattleSimulator() const { return _battleSimulator; }

	BattleCommander* GetCommander() const { return _commander; }
	void SetCommander(BattleCommander* value) { _commander = value; }

	// BattleObserver
	virtual void OnSetGroundMap(GroundMap* groundMap);
	virtual void OnAddUnit(Unit* unit);
	virtual void OnRemoveUnit(Unit* unit);
	virtual void OnShooting(const Shooting& shooting);
	virtual void OnCasualty(const Fighter& fighter);

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

	void InitializeCameraPosition(const std::vector<Unit*>& units);

	virtual void Render();
	virtual void Update(double secondsSinceLastUpdate);

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
