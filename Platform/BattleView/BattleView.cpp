// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shapes/BillboardColorShader.h"
#include "Surface/Surface.h"
#include "BattleModel/BattleCommander.h"
#include "SmoothTerrain/SmoothTerrainWater.h"
#include "SmoothTerrain/SmoothTerrainSky.h"
#include "BattleView.h"
#include "CasualtyMarker.h"
#include "RangeMarker.h"
#include "ShootingCounter.h"
#include "SmokeCounter.h"
#include "UnitCounter.h"
#include "UnitMovementMarker.h"
#include "UnitTrackingMarker.h"
#include "Graphics/TextureResource.h"
#include "BattleHotspot.h"
#include "TerrainView/TerrainViewport.h"
#include "Audio/SoundPlayer.h"

#include <glm/gtc/matrix_transform.hpp>



static affine2 billboard_texcoords(int x, int y, bool flip)
{
	float x0 = 0.125f * x;
	float x1 = x0 + 0.125f;
	float y0 = 0.125f * y;
	float y1 = y0 + 0.125f;
	return flip ? affine2(glm::vec2(x1, y0), glm::vec2(x0, y1)) : affine2(glm::vec2(x0, y0), glm::vec2(x1, y1));
}



BattleView::BattleView(Surface* surface) : TerrainView(surface),
	_gc(surface->GetGraphicsContext()),
	_simulator(nullptr),
	_commander(nullptr),
	_lightNormal(),
	_billboardTexture(nullptr),
	_billboardModel(nullptr),
	_textureBillboardShape(nullptr),
	_textureBillboardShape1(nullptr),
	_textureBillboardShape2(nullptr),
	_casualtyMarker(0),
	_movementMarkers(),
	_trackingMarkers(),
	_plainLineVertices(nullptr),
	_gradientLineVertices(nullptr),
	_gradientTriangleVertices(nullptr),
	_gradientTriangleStripVertices(nullptr),
	_colorBillboardVertices(nullptr),
	_textureTriangleVertices(nullptr),
	_textureTriangleVertices2(nullptr),
	_textureUnitMarkers(nullptr),
	_textureTouchMarker(nullptr),
	_smoothTerrainSurface(nullptr),
	_smoothTerrainWater(nullptr),
	_smoothTerrainSky(nullptr),
	_tiledTerrainRenderer(nullptr)
{
	_textureUnitMarkers = new TextureResource(_gc, resource("Textures/UnitMarkers.png"));
	_textureTouchMarker = new TextureResource(_gc, resource("Textures/TouchMarker.png"));

	_billboardTexture = new BillboardTextureSheet(_gc);

	Image image;
	image.LoadFromResource(resource("Textures/Billboards.png"));
#ifndef OPENWAR_USE_NSBUNDLE_RESOURCES
	image.PremultiplyAlpha(); // TODO: how to handle this ???
#endif
	_billboardTexture->AddSheet(image);

	_billboardModel = new BillboardModel();
	_billboardModel->texture = _billboardTexture;


	for (int i = 0; i < 8; ++i)
	{
		_billboardModel->_billboardTreeShapes[i] = _billboardTexture->AddShape();
		_billboardTexture->SetTexCoords(_billboardModel->_billboardTreeShapes[i], 0, billboard_texcoords(i, 0, false));

		_billboardModel->_billboardTreeShapes[i + 8] = _billboardTexture->AddShape();
		_billboardTexture->SetTexCoords(_billboardModel->_billboardTreeShapes[i + 8], 0, billboard_texcoords(i, 0, true));

		_billboardModel->_billboardShapeCasualtyCav[i] = _billboardTexture->AddShape();
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyCav[i], 0, billboard_texcoords(i, 4, false));

		_billboardModel->_billboardShapeCasualtyCav[i + 8] = _billboardTexture->AddShape();
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyCav[i + 8], 0, billboard_texcoords(i, 4, true));
	}

	for (int i = 0; i < 4; ++i)
	{
		_billboardModel->_billboardShapeCasualtyAsh[i] = _billboardTexture->AddShape();
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyAsh[i], 0, billboard_texcoords(i, 3, false));

		_billboardModel->_billboardShapeCasualtyAsh[i + 4] = _billboardTexture->AddShape();
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyAsh[i + 4], 0, billboard_texcoords(i, 3, false));

		_billboardModel->_billboardShapeCasualtySam[i] = _billboardTexture->AddShape();
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtySam[i], 0, billboard_texcoords(i + 4, 3, false));

		_billboardModel->_billboardShapeCasualtySam[i + 4] = _billboardTexture->AddShape();
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtySam[i + 4], 0, billboard_texcoords(i + 4, 3, false));
	}

	_billboardModel->_billboardShapeFighterSamBlue = _billboardTexture->AddShape();
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamBlue, 0, billboard_texcoords(1, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamBlue, 180, billboard_texcoords(2, 1, false));

	_billboardModel->_billboardShapeFighterSamRed = _billboardTexture->AddShape();
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamRed, 0, billboard_texcoords(1, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamRed, 180, billboard_texcoords(2, 2, false));

	_billboardModel->_billboardShapeFighterAshBlue = _billboardTexture->AddShape();
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshBlue, 0, billboard_texcoords(3, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshBlue, 180, billboard_texcoords(4, 1, false));

	_billboardModel->_billboardShapeFighterAshRed = _billboardTexture->AddShape();
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshRed, 0, billboard_texcoords(3, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshRed, 180, billboard_texcoords(4, 2, false));

	_billboardModel->_billboardShapeFighterCavBlue = _billboardTexture->AddShape();
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue,  45, billboard_texcoords(7, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue,  90, billboard_texcoords(6, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 135, billboard_texcoords(5, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 225, billboard_texcoords(5, 1, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 270, billboard_texcoords(6, 1, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 315, billboard_texcoords(7, 1, true));

	_billboardModel->_billboardShapeFighterCavRed = _billboardTexture->AddShape();
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed,  45, billboard_texcoords(7, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed,  90, billboard_texcoords(6, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 135, billboard_texcoords(5, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 225, billboard_texcoords(5, 2, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 270, billboard_texcoords(6, 2, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 315, billboard_texcoords(7, 2, true));

	for (int i = 0; i < 8; ++i)
	{
		_billboardModel->_billboardShapeSmoke[i] = _billboardTexture->AddShape();
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeSmoke[i], 0, billboard_texcoords(i, 7, false));
	}

	_textureBillboardShape = new BillboardTextureShape();
	_textureBillboardShape1 = new BillboardTextureShape();
	_textureBillboardShape2 = new BillboardTextureShape();

	_plainLineVertices = new VertexShape_3f();
	_gradientLineVertices = new VertexShape_3f_4f();
	_gradientTriangleVertices = new VertexShape_3f_4f();
	_gradientTriangleStripVertices = new VertexShape_3f_4f();
	_colorBillboardVertices = new VertexShape_3f_4f_1f();
	_textureTriangleVertices = new VertexShape_3f_2f();
	_textureTriangleVertices2 = new VertexShape_2f_2f();

	_smoothTerrainSky = new SmoothTerrainSky(_gc);
}


BattleView::~BattleView()
{
	delete _casualtyMarker;

	for (UnitMovementMarker* marker : _movementMarkers)
		delete marker;

	for (UnitTrackingMarker* marker : _trackingMarkers)
		delete marker;


	delete _textureUnitMarkers;
	delete _textureTouchMarker;

	delete _billboardTexture;
	delete _billboardModel;

	delete _textureBillboardShape;
	delete _textureBillboardShape1;
	delete _textureBillboardShape2;

	delete _plainLineVertices;
	delete _gradientLineVertices;
	delete _gradientTriangleVertices;
	delete _gradientTriangleStripVertices;
	delete _colorBillboardVertices;
	delete _textureTriangleVertices;
	delete _textureTriangleVertices2;

	delete _smoothTerrainSurface;
	delete _smoothTerrainWater;
	delete _smoothTerrainSky;
	delete _tiledTerrainRenderer;

	for (ShootingCounter* shootingCounter : _shootingCounters)
		delete shootingCounter;

	for (SmokeCounter* marker : _smokeMarkers)
		delete marker;

	for (UnitCounter* marker : _unitMarkers)
		delete marker;
}


void BattleView::SetSimulator(BattleSimulator* simulator)
{
	if (simulator == _simulator)
		return;

	if (_simulator != nullptr)
	{
		for (Unit* unit : _simulator->GetUnits())
			OnRemoveUnit(unit);
		_simulator->RemoveObserver(this);
	}

	_simulator = simulator;
	SetHeightMap(simulator->GetHeightMap());

	delete _casualtyMarker;
	_casualtyMarker = new CasualtyMarker(_simulator);

	_simulator->AddObserver(this);
}


static bool ShouldEnableRenderEdges(GraphicsContext* gc)
{
#if TARGET_OS_IPHONE
	return gc->GetNativeScaling() > 1;
#else
	return true;
#endif
}


void BattleView::OnSetGroundMap(GroundMap* groundMap)
{
	SmoothGroundMap*  smoothGroundMap = dynamic_cast<SmoothGroundMap*>(groundMap);
	if (smoothGroundMap == nullptr)
		return;

	/*if (smoothGroundMap != nullptr
		&& _smoothTerrainSurface != nullptr
		&& smoothGroundMap->GetImage() == _smoothTerrainSurface->GetSmoothGroundMap()->GetImage())
	{
		return; // no image, same map
	}*/

	delete _smoothTerrainSurface;
	_smoothTerrainSurface = nullptr;

	delete _smoothTerrainWater;
	_smoothTerrainWater = nullptr;

	delete _tiledTerrainRenderer;
	_tiledTerrainRenderer = nullptr;

	if (smoothGroundMap != nullptr)
	{
		_smoothTerrainSurface = new SmoothTerrainRenderer(_gc, smoothGroundMap);
		_smoothTerrainWater = new SmoothTerrainWater(smoothGroundMap);

		if (ShouldEnableRenderEdges(_gc))
			_smoothTerrainSurface->EnableRenderEdges();
	}

	TiledGroundMap* tiledGroundMap = dynamic_cast<TiledGroundMap*>(groundMap);
	if (tiledGroundMap != nullptr)
	{
		tiledGroundMap->UpdateHeightMap();
		_tiledTerrainRenderer = new TiledTerrainRenderer(_gc, tiledGroundMap);
	}

	UpdateTerrainTrees(groundMap->GetBounds());
}


void BattleView::OnAddUnit(Unit* unit)
{
	UnitCounter* marker = new UnitCounter(this, unit);
	marker->Animate(0);
	_unitMarkers.push_back(marker);
}


void BattleView::OnRemoveUnit(Unit* unit)
{
	for (auto i = _unitMarkers.begin(); i != _unitMarkers.end(); ++i)
		if ((*i)->GetUnit() == unit)
		{
			delete *i;
			_unitMarkers.erase(i);
			break;
		}

	for (auto i = _movementMarkers.begin(); i != _movementMarkers.end(); ++i)
		if ((*i)->GetUnit() == unit)
		{
			delete *i;
			_movementMarkers.erase(i);
			break;
		}

	for (auto i = _trackingMarkers.begin(); i != _trackingMarkers.end(); ++i)
		if ((*i)->GetUnit() == unit)
		{
			delete *i;
			_trackingMarkers.erase(i);
			break;
		}
}


void BattleView::OnCommand(Unit* unit, float timer)
{
	if (unit->IsFriendlyCommander(_commander) && GetMovementMarker(unit) == nullptr)
		AddMovementMarker(unit);
}


void BattleView::OnShooting(Shooting const & shooting, float timer)
{
}


void BattleView::OnRelease(const Shooting& shooting)
{
	AddShootingAndSmokeCounters(shooting);
}


void BattleView::OnCasualty(const Fighter& fighter)
{
	AddCasualty(fighter.unit, fighter.state.position);
}


void BattleView::OnRouting(Unit* unit)
{
}


void BattleView::AddCasualty(Unit* unit, glm::vec2 position)
{
	glm::vec3 p = glm::vec3(position, _simulator->GetHeightMap()->InterpolateHeight(position));
	SamuraiPlatform platform = SamuraiModule::GetSamuraiPlatform(unit->unitClass.c_str());
	_casualtyMarker->AddCasualty(p, unit->commander->GetTeam(), platform);
}


static float random_float()
{
	return (float)(rand() & 0x7fff) / 0x7fff;
}


void BattleView::Initialize()
{
	bounds2f terrainBounds = _simulator->GetHeightMap()->GetBounds();

	GetViewport()->SetBounds(GetBounds());
	GetViewport()->SetTerrainBounds(_simulator->GetHeightMap()->GetBounds());
	GetViewport()->SetCameraPosition(glm::vec3(terrainBounds.mid(), 0.3f * glm::length(terrainBounds.size())));

	InitializeTerrainTrees();
	InitializeCameraPosition();

	OnSetGroundMap(_simulator->GetGroundMap());
}


void BattleView::InitializeTerrainTrees()
{
	UpdateTerrainTrees(_simulator->GetHeightMap()->GetBounds());
}


struct random_generator
{
	int _count;
	float* _values;

	random_generator(int count) : _count(count), _values(new float [count])
	{
		for (int i = 0; i < count; ++i)
			_values[i] = random_float();
	}
};

struct random_iterator
{
	const random_generator& _gen;
	int _index;
	random_iterator(const random_generator& gen) : _gen(gen), _index(0) { }
	float next()
	{
		float result = _gen._values[_index++];
		if (_index == _gen._count)
			_index = 0;
		return result;
	}
};


void BattleView::UpdateTerrainTrees(bounds2f bounds)
{
	if (_smoothTerrainSurface != nullptr && _simulator->GetGroundMap() != nullptr)
	{
		auto pos2 = std::remove_if(_billboardModel->staticBillboards.begin(), _billboardModel->staticBillboards.end(), [bounds](const Billboard& billboard) {
			return bounds.contains(billboard.position.xy());
		});
		_billboardModel->staticBillboards.erase(pos2, _billboardModel->staticBillboards.end());


		static random_generator* _randoms = nullptr;
		if (_randoms == nullptr)
			_randoms = new random_generator(997);

		int treeType = 0;
		random_iterator random(*_randoms);
		bounds2f mapbounds = _simulator->GetGroundMap()->GetBounds();
		glm::vec2 center = mapbounds.mid();
		float radius = mapbounds.x().size() / 2;

		float d = 5 * mapbounds.x().size() / 1024;
		for (float x = mapbounds.min.x; x < mapbounds.max.x; x += d)
			for (float y = mapbounds.min.y; y < mapbounds.max.y; y += d)
			{
				float dx = d * (random.next() - 0.5f);
				float dy = d * (random.next() - 0.5f);
				int shape = (int)(15 * random.next()) & 15;

				glm::vec2 position = glm::vec2(x + dx, y + dy);
				if (bounds.contains(position) && glm::distance(position, center) < radius)
				{
					if (_simulator->GetHeightMap()->InterpolateHeight(position) > 0 && _simulator->GetGroundMap()->IsForest(position))
					{
						const float adjust = 0.5 - 2.0 / 64.0; // place texture 2 texels below ground
						_billboardModel->staticBillboards.push_back(Billboard(GetTerrainPosition(position, adjust * 5), 0, 5, _billboardModel->_billboardTreeShapes[shape]));
					}
				}

				++treeType;
			}
	}
}



void BattleView::InitializeCameraPosition()
{
	glm::vec2 friendlyCenter;
	glm::vec2 enemyCenter;

	int position = _commander != nullptr && _commander->GetType() == BattleCommanderType::Player ? _commander->GetTeamPosition() : 0;
	switch (position)
	{
		case 1:
			friendlyCenter = glm::vec2(512, 512 - 400);
			enemyCenter = glm::vec2(512, 512 + 64);
			break;

		case 2:
			friendlyCenter = glm::vec2(512, 512 + 400);
			enemyCenter = glm::vec2(512, 512 - 64);
			break;
		default:
			friendlyCenter = glm::vec2(512 - 400, 512);
			enemyCenter = glm::vec2(512 + 64, 512);
			break;
	}

	bool flip = GetViewport()->GetFlip();

	glm::vec2 friendlyScreen = GetViewport()->NormalizedToLocal(glm::vec2(0, flip ? 0.4 : -0.4));
	glm::vec2 enemyScreen = GetViewport()->NormalizedToLocal(glm::vec2(0, flip ? -0.4 : 0.4));

	Zoom(GetTerrainPosition(friendlyCenter, 0), GetTerrainPosition(enemyCenter, 0), friendlyScreen, enemyScreen, 0);

	//ClampCameraPosition();
}


void BattleView::Render()
{
	GetViewport()->SetBounds(GetBounds());
	GetViewport()->UseViewport();

	glm::mat4 transform = GetViewport()->GetTransform();


	glm::vec2 facing = vector2_from_angle(GetViewport()->GetCameraFacing() - 2.5f * (float)M_PI_4);
	_lightNormal = glm::normalize(glm::vec3(facing, -1));

	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);


	// Terrain Sky

	if (_smoothTerrainSky != nullptr)
	{
		_smoothTerrainSky->RenderBackgroundLinen(GetBounds());
		_smoothTerrainSky->Render(GetViewport()->GetCameraDirection().z);
	}


	// Terrain Surface

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (_smoothTerrainSurface != nullptr)
		_smoothTerrainSurface->Render(transform, _lightNormal);

	if (_tiledTerrainRenderer != nullptr)
		_tiledTerrainRenderer->Render(transform, _lightNormal);


	// Terrain Water

	glDisable(GL_CULL_FACE);
	if (_smoothTerrainWater != nullptr)
		_smoothTerrainWater->Render(_gc, transform);


	// Fighter Weapons

	glDepthMask(false);
	_plainLineVertices->Reset(GL_LINES);
	for (UnitCounter* marker : _unitMarkers)
		marker->AppendFighterWeapons(_plainLineVertices);

	glLineWidth(1);
	RenderCall<PlainShader_3f>(_gc)
		.SetVertices(_plainLineVertices, "position")
		.SetUniform("transform", transform)
		.SetUniform("point_size", 1)
		.SetUniform("color", glm::vec4(0.4, 0.4, 0.4, 0.6))
		.Render();


	// Color Billboards

	_colorBillboardVertices->Reset(GL_POINTS);
	_casualtyMarker->RenderCasualtyColorBillboards(_colorBillboardVertices);

	RenderCall<BillboardColorShader>(_gc)
		.SetVertices(_colorBillboardVertices, "position", "color", "height")
		.SetUniform("transform", transform)
		.SetUniform("upvector", GetViewport()->GetCameraUpVector())
		.SetUniform("viewport_height", 0.25f * _gc->GetCombinedScaling() * GetViewport()->GetBounds().y().size())
		.Render();


	// Texture Billboards

	_billboardModel->dynamicBillboards.clear();
	_casualtyMarker->AppendCasualtyBillboards(_billboardModel);
	for (UnitCounter* marker : _unitMarkers)
		marker->AppendFighterBillboards(_billboardModel);
	for (SmokeCounter* marker : _smokeMarkers)
		marker->AppendSmokeBillboards(_billboardModel);
	_textureBillboardShape->Render(_gc,
		_billboardModel,
		transform,
		GetViewport()->GetCameraUpVector(),
		glm::degrees(GetViewport()->GetCameraFacing()),
		GetViewport()->GetBounds().y().size(),
		GetViewport()->GetFlip());


	// Range Markers

	for (Unit* unit : _simulator->GetUnits())
	{
		if (unit->IsFriendlyCommander(_commander))
		{
			RangeMarker marker(_simulator, unit);
			_gradientTriangleStripVertices->Reset(GL_TRIANGLE_STRIP);
			marker.Render(_gradientTriangleStripVertices);

			RenderCall<GradientShader_3f>(_gc)
				.SetVertices(_gradientTriangleStripVertices, "position", "color")
				.SetUniform("transform", transform)
				.SetUniform("point_size", 1)
				.Render();
		}
	}


	// Unit Facing Markers

	glDisable(GL_DEPTH_TEST);
	_textureTriangleVertices2->Reset(GL_TRIANGLES);

	for (UnitCounter* marker : _unitMarkers)
		if (marker->GetUnit()->IsFriendlyCommander(_commander))
			marker->AppendFacingMarker(_textureTriangleVertices2, this);
	for (UnitMovementMarker* marker : _movementMarkers)
		if (marker->GetUnit()->IsFriendlyCommander(_commander))
			marker->AppendFacingMarker(_textureTriangleVertices2, this);
	for (UnitTrackingMarker* marker : _trackingMarkers)
		if (marker->GetUnit()->IsFriendlyCommander(_commander))
			marker->AppendFacingMarker(_textureTriangleVertices2, this);

	RenderCall<TextureShader_2f>(_gc)
		.SetVertices(_textureTriangleVertices2, "position", "texcoord")
		.SetUniform("transform", glm::mat4())
		.SetTexture("texture", _textureUnitMarkers)
		.Render();



	// Unit Markers

	_textureBillboardShape1->Reset();
	_textureBillboardShape2->Reset();

	for (UnitCounter* marker : _unitMarkers)
		marker->AppendUnitMarker(_textureBillboardShape2, GetViewport()->GetFlip());
	for (UnitMovementMarker* marker : _movementMarkers)
		marker->RenderMovementMarker(_textureBillboardShape1);
	for (UnitTrackingMarker* marker : _trackingMarkers)
		marker->RenderTrackingMarker(_textureBillboardShape1);

	bounds1f sizeLimit = GetUnitIconSizeLimit();

	_textureBillboardShape1->Draw(_gc,
		_textureUnitMarkers,
		transform,
		GetViewport()->GetCameraUpVector(),
		glm::degrees(GetViewport()->GetCameraFacing()),
		GetViewport()->GetBounds().y().size(),
		sizeLimit);

	_textureBillboardShape2->Draw(_gc,
		_textureUnitMarkers,
		transform,
		GetViewport()->GetCameraUpVector(),
		glm::degrees(GetViewport()->GetCameraFacing()),
		GetViewport()->GetBounds().y().size(),
		sizeLimit);


	// Tracking Markers

	glDisable(GL_DEPTH_TEST);
	for (UnitTrackingMarker* marker : _trackingMarkers)
	{
		_textureBillboardShape1->Reset();
		marker->RenderTrackingShadow(_textureBillboardShape1);
		_textureBillboardShape1->Draw(_gc,
			_textureTouchMarker,
			transform,
			GetViewport()->GetCameraUpVector(),
			glm::degrees(GetViewport()->GetCameraFacing()),
			GetViewport()->GetBounds().y().size(),
			bounds1f(64, 64));
	}


	// Movement Paths

	glEnable(GL_DEPTH_TEST);
	_gradientTriangleVertices->Reset(GL_TRIANGLES);
	for (UnitMovementMarker* marker : _movementMarkers)
		marker->RenderMovementPath(_gradientTriangleVertices);

	RenderCall<GradientShader_3f>(_gc)
		.SetVertices(_gradientTriangleVertices, "position", "color")
		.SetUniform("transform", transform)
		.SetUniform("point_size", 1)
		.Render();


	// Tracking Path

	glDisable(GL_DEPTH_TEST);
	for (UnitTrackingMarker* marker : _trackingMarkers)
	{
		_gradientTriangleVertices->Reset(GL_TRIANGLES);
		marker->RenderTrackingPath(_gradientTriangleVertices);
		marker->RenderOrientation(_gradientTriangleVertices);

		RenderCall<GradientShader_3f>(_gc)
			.SetVertices(_gradientTriangleVertices, "position", "color")
			.SetUniform("transform", transform)
			.SetUniform("point_size", 1)
			.Render();
	}


	// Tracking Fighters

	glEnable(GL_DEPTH_TEST);
	_colorBillboardVertices->Reset(GL_POINTS);
	for (UnitTrackingMarker* marker : _trackingMarkers)
		marker->RenderTrackingFighters(_colorBillboardVertices);

	RenderCall<BillboardColorShader>(_gc)
		.SetVertices(_colorBillboardVertices, "position", "color", "height")
		.SetUniform("transform", transform)
		.SetUniform("upvector", GetViewport()->GetCameraUpVector())
		.SetUniform("viewport_height", 0.25f * _gc->GetCombinedScaling() * GetViewport()->GetBounds().y().size())
		.Render();


	// Movement Fighters

	_colorBillboardVertices->Reset(GL_POINTS);
	for (UnitMovementMarker* marker : _movementMarkers)
		marker->RenderMovementFighters(_colorBillboardVertices);

	RenderCall<BillboardColorShader>(_gc)
		.SetVertices(_colorBillboardVertices, "position", "color", "height")
		.SetUniform("transform", transform)
		.SetUniform("upvector", GetViewport()->GetCameraUpVector())
		.SetUniform("viewport_height", 0.25f * _gc->GetCombinedScaling() * GetViewport()->GetBounds().y().size())
		.Render();


	// Shooting Counters

	_gradientLineVertices->Reset(GL_LINES);
	for (ShootingCounter* shootingCounter : _shootingCounters)
		shootingCounter->Render(_gradientLineVertices);

	RenderCall<GradientShader_3f>(_gc)
		.SetVertices(_gradientLineVertices, "position", "color")
		.SetUniform("transform", transform)
		.SetUniform("point_size", 1)
		.Render();


	// Mouse Hint

	_plainLineVertices->Reset(GL_LINES);
	RenderMouseHint(_plainLineVertices);

	glLineWidth(1);
	RenderCall<PlainShader_3f>(_gc)
		.SetVertices(_plainLineVertices, "position")
		.SetUniform("transform", transform)
		.SetUniform("point_size", 1)
		.SetUniform("color", glm::vec4(0, 0, 0, 0.5f))
		.Render();


	glDepthMask(true);
	glDisable(GL_DEPTH_TEST);
}


template <class T> void AnimateMarkers(std::vector<T*>& markers, float seconds)
{
	size_t index = 0;
	while (index < markers.size())
	{
		T* marker = markers[index];
		if (marker->Animate(seconds))
		{
			++index;
		}
		else
		{
			markers.erase(markers.begin() + index);
			delete marker;
		}
	}
}




void BattleView::OnRenderLoop(double secondsSinceLastUpdate)
{
	UpdateSoundPlayer();

	_casualtyMarker->Animate((float)secondsSinceLastUpdate);

	::AnimateMarkers(_movementMarkers, (float)secondsSinceLastUpdate);
	::AnimateMarkers(_unitMarkers, (float)secondsSinceLastUpdate);
	::AnimateMarkers(_shootingCounters, (float)secondsSinceLastUpdate);
	::AnimateMarkers(_smokeMarkers, (float)secondsSinceLastUpdate);
}


void BattleView::OnTouchBegin(Touch* touch)
{
	if (_battleHotspot == nullptr)
		_battleHotspot = std::make_shared<BattleHotspot>(this);
	_battleHotspot->SubscribeTouch(touch);

	TerrainView::OnTouchBegin(touch);
}


UnitMovementMarker* BattleView::AddMovementMarker(Unit* unit)
{
	UnitMovementMarker* marker = new UnitMovementMarker(this, unit);
	_movementMarkers.push_back(marker);
	return marker;
}


UnitMovementMarker* BattleView::GetMovementMarker(Unit* unit)
{
	for (UnitMovementMarker* marker : _movementMarkers)
		if (marker->GetUnit() == unit)
			return marker;

	return nullptr;
}


UnitMovementMarker* BattleView::GetNearestMovementMarker(glm::vec2 position, BattleCommander* commander)
{
	UnitMovementMarker* result = 0;
	float nearest = INFINITY;

	for (UnitMovementMarker* marker : _movementMarkers)
	{
		Unit* unit = marker->GetUnit();
		if (commander != nullptr && !unit->IsCommandableBy(commander))
			continue;

		const UnitCommand& command = unit->GetCommand();
		glm::vec2 p = command.GetDestination();
		float dx = p.x - position.x;
		float dy = p.y - position.y;
		float d = dx * dx + dy * dy;
		if (d < nearest)
		{
			result = marker;
			nearest = d;
		}
	}

	return result;
}


UnitTrackingMarker* BattleView::AddTrackingMarker(Unit* unit)
{
	UnitTrackingMarker* trackingMarker = new UnitTrackingMarker(this, unit);
	_trackingMarkers.push_back(trackingMarker);
	return trackingMarker;
}


UnitTrackingMarker* BattleView::GetTrackingMarker(Unit* unit)
{
	for (UnitTrackingMarker* marker : _trackingMarkers)
		if (marker->GetUnit() == unit)
			return marker;

	return 0;
}


void BattleView::RemoveTrackingMarker(UnitTrackingMarker* trackingMarker)
{
	auto i = std::find(_trackingMarkers.begin(), _trackingMarkers.end(), trackingMarker);
	if (i != _trackingMarkers.end())
	{
		_trackingMarkers.erase(i);
		delete trackingMarker;
	}
}



static bool is_iphone()
{
	static bool* _is_iphone = nullptr;
	if (_is_iphone == nullptr)
	{
#if TARGET_OS_IPHONE
		_is_iphone = new bool([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone);
#else
		_is_iphone = new bool(false);
#endif
	}
	return *_is_iphone;
}


bounds2f BattleView::GetBillboardBounds(glm::vec3 position, float height)
{
	glm::mat4x4 transform = GetViewport()->GetTransform();
	glm::vec3 upvector = GetViewport()->GetCameraUpVector();
	float viewport_height = GetViewport()->GetBounds().y().size();
	bounds1f sizeLimit = GetUnitIconSizeLimit() / (_gc->GetCombinedScaling());

	glm::vec3 position2 = position + height * 0.5f * viewport_height * upvector;
	glm::vec4 p = transform * glm::vec4(position, 1);
	glm::vec4 q = transform * glm::vec4(position2, 1);
	float s = glm::clamp(glm::abs(q.y / q.w - p.y / p.w), sizeLimit.min, sizeLimit.max);

	return bounds2f(GetViewport()->NormalizedToLocal((glm::vec2)p.xy() / p.w)).grow(s / 2);
}


bounds2f BattleView::GetUnitCurrentIconViewportBounds(Unit* unit)
{
	glm::vec3 position = GetTerrainPosition(unit->state.center, 0);
	return GetBillboardBounds(position, 32);
}


bounds2f BattleView::GetUnitFutureIconViewportBounds(Unit* unit)
{
	const UnitCommand& command = unit->GetCommand();
	glm::vec3 position = GetTerrainPosition(!command.path.empty() ? command.path.back() : unit->state.center, 0);
	return GetBillboardBounds(position, 32);
}


bounds2f BattleView::GetUnitFacingMarkerBounds(glm::vec2 center, float direction)
{
	bounds2f iconBounds = GetBillboardBounds(GetTerrainPosition(center, 0), 32);

	glm::vec2 position = iconBounds.mid();
	float size = iconBounds.y().size();
	float adjust = GetViewport()->GetFlip() ? 3 * glm::half_pi<float>() : glm::half_pi<float>();

	position += 0.7f * size * vector2_from_angle(direction - GetViewport()->GetCameraFacing() + adjust);

	return bounds2f(position).grow(0.2f * size);
}


bounds2f BattleView::GetUnitCurrentFacingMarkerBounds(Unit* unit)
{
	return GetUnitFacingMarkerBounds(unit->state.center, unit->state.bearing);
}


bounds2f BattleView::GetUnitFutureFacingMarkerBounds(Unit* unit)
{
	const UnitCommand& command = unit->GetCommand();

	glm::vec2 center = !command.path.empty() ? command.path.back() : unit->state.center;

	return GetUnitFacingMarkerBounds(center, command.bearing);
}


bounds1f BattleView::GetUnitIconSizeLimit() const
{
	float y = GetViewport()->GetCameraDirection().z;
	float x = sqrtf(1 - y * y);
	float a = 1 - fabsf(atan2f(y, x) / (float)M_PI_2);

	float pixelsPerPoint = _gc->GetCombinedScaling();

	bounds1f result(0, 0);
	result.min = (32 - 8 * a) * pixelsPerPoint;
	result.max = (32 + 16 * a) * pixelsPerPoint;
	if (is_iphone())
	{
		result.min *= 57.0f / 72.0f;
		result.max *= 57.0f / 72.0f;
	}
	return result;
}


/***/



void BattleView::AddShootingAndSmokeCounters(const Shooting& shooting)
{
	AddShootingCounter(shooting);
	if (shooting.missileType == MissileType::Arq)
		AddSmokeMarker(shooting);
}


void BattleView::AddShootingCounter(const Shooting& shooting)
{
	ShootingCounter* shootingCounter = AddShootingCounter(shooting.missileType);

	for (const Projectile& projectile : shooting.projectiles)
	{
		glm::vec3 p1 = glm::vec3(projectile.position1, _simulator->GetHeightMap()->InterpolateHeight(projectile.position1));
		glm::vec3 p2 = glm::vec3(projectile.position2, _simulator->GetHeightMap()->InterpolateHeight(projectile.position2));
		shootingCounter->AddProjectile(p1, p2, projectile.delay, shooting.timeToImpact);
	}
}


ShootingCounter* BattleView::AddShootingCounter(MissileType missileType)
{
	ShootingCounter* shootingCounter = new ShootingCounter(missileType);
	_shootingCounters.push_back(shootingCounter);
	return shootingCounter;
}


void BattleView::RemoveAllShootingMarkers()
{
	for (ShootingCounter* shootingCounters : _shootingCounters)
	{
		shootingCounters->Animate(100);
	}
}


void BattleView::AddSmokeMarker(const Shooting& shooting)
{
	SmokeCounter* marker = AddSmokeMarker(shooting.missileType);

	for (const Projectile& projectile : shooting.projectiles)
	{
		glm::vec3 p1 = glm::vec3(projectile.position1, _simulator->GetHeightMap()->InterpolateHeight(projectile.position1));
		glm::vec3 p2 = glm::vec3(projectile.position2, _simulator->GetHeightMap()->InterpolateHeight(projectile.position2));
		marker->AddParticle(p1, p2, projectile.delay);
	}
}


SmokeCounter* BattleView::AddSmokeMarker(MissileType missileType)
{
	SmokeCounter* marker = new SmokeCounter(missileType);
	_smokeMarkers.push_back(marker);
	return marker;
}


void BattleView::RemoveAllSmokeMarkers()
{
	for (SmokeCounter* marker : _smokeMarkers)
	{
		marker->Animate(100);
	}
}


UnitCounter* BattleView::GetNearestUnitCounter(glm::vec2 position, int team, BattleCommander* commander)
{
	UnitCounter* result = 0;
	float nearest = INFINITY;

	for (UnitCounter* marker : _unitMarkers)
	{
		Unit* unit = marker->_unit;
		if (team != 0 && unit->commander->GetTeam() != team)
			continue;
		if (commander != nullptr && !unit->IsCommandableBy(commander))
			continue;

		glm::vec2 p = unit->state.center;
		float dx = p.x - position.x;
		float dy = p.y - position.y;
		float d = dx * dx + dy * dy;
		if (d < nearest)
		{
			result = marker;
			nearest = d;
		}
	}

	return result;
}


void BattleView::UpdateSoundPlayer()
{
	int horseGallop = 0;
	int horseTrot = 0;
	int fighting = 0;
	int infantryMarching = 0;
	int infantryRunning = 0;

	for (UnitCounter* unitMarker : GetUnitCounters())
	{
		Unit* unit = unitMarker->_unit;
		if (glm::length(unit->command.GetDestination() - unit->state.center) > 4.0f)
		{
			if (unit->stats.platformType == PlatformType::Cavalry)
			{
				if (unit->command.running)
					++horseGallop;
				else
					++horseTrot;
			}
			else
			{
				if (unit->command.running)
					++infantryRunning;
				else
					++infantryMarching;
			}
		}

		if (unit->command.meleeTarget != nullptr)
			++fighting;
	}

	SoundPlayer::singleton->UpdateInfantryWalking(infantryMarching != 0);
	SoundPlayer::singleton->UpdateInfantryRunning(infantryRunning != 0);

	SoundPlayer::singleton->UpdateCavalryWalking(horseTrot != 0);
	SoundPlayer::singleton->UpdateCavalryRunning(horseGallop != 0);

	SoundPlayer::singleton->UpdateFighting(_simulator->IsMelee());
}
