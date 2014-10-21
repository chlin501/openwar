// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ColorBillboardShader.h"
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

#include <glm/gtc/matrix_transform.hpp>



static affine2 billboard_texcoords(int x, int y, bool flip)
{
	float x0 = 0.125f * x;
	float x1 = x0 + 0.125f;
	float y0 = 0.125f * y;
	float y1 = y0 + 0.125f;
	return flip ? affine2(glm::vec2(x1, y0), glm::vec2(x0, y1)) : affine2(glm::vec2(x0, y0), glm::vec2(x1, y1));
}



BattleView::BattleView(GraphicsContext* gc) :
	_simulator(nullptr),
	_commander(nullptr),
	_lightNormal(),
	_billboardTexture(nullptr),
	_billboardModel(nullptr),
	_textureBillboardRenderer(nullptr),
	_textureBillboardRenderer1(nullptr),
	_textureBillboardRenderer2(nullptr),
	_casualtyMarker(0),
	_movementMarkers(),
	_trackingMarkers(),
	_plainLineVertices(nullptr),
	_gradientLineVertices(nullptr),
	_gradientTriangleVertices(nullptr),
	_gradientTriangleStripVertices(nullptr),
	_colorBillboardVertices(nullptr),
	_textureTriangleVertices(nullptr),
	_textureUnitMarkers(nullptr),
	_textureTouchMarker(nullptr),
	_smoothTerrainSurface(nullptr),
	_smoothTerrainWater(nullptr),
	_smoothTerrainSky(nullptr),
	_tiledTerrainRenderer(nullptr)
{
	SetUsingDepth(true);

	_textureUnitMarkers = new texture(resource("Textures/UnitMarkers.png"));
	_textureTouchMarker = new texture(resource("Textures/TouchMarker.png"));

	_billboardTexture = new BillboardTexture();

	Image img(resource("Textures/Billboards.png"));
#ifndef OPENWAR_USE_NSBUNDLE_RESOURCES
	img.premultiply_alpha(); // TODO: how to handle this ???
#endif
	_billboardTexture->AddSheet(img);

	_billboardModel = new BillboardModel();
	_billboardModel->texture = _billboardTexture;


	for (int i = 0; i < 8; ++i)
	{
		_billboardModel->_billboardTreeShapes[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardTreeShapes[i], 0, billboard_texcoords(i, 0, false));

		_billboardModel->_billboardTreeShapes[i + 8] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardTreeShapes[i + 8], 0, billboard_texcoords(i, 0, true));

		_billboardModel->_billboardShapeCasualtyCav[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyCav[i], 0, billboard_texcoords(i, 4, false));

		_billboardModel->_billboardShapeCasualtyCav[i + 8] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyCav[i + 8], 0, billboard_texcoords(i, 4, true));
	}

	for (int i = 0; i < 4; ++i)
	{
		_billboardModel->_billboardShapeCasualtyAsh[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyAsh[i], 0, billboard_texcoords(i, 3, false));

		_billboardModel->_billboardShapeCasualtyAsh[i + 4] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtyAsh[i + 4], 0, billboard_texcoords(i, 3, false));

		_billboardModel->_billboardShapeCasualtySam[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtySam[i], 0, billboard_texcoords(i + 4, 3, false));

		_billboardModel->_billboardShapeCasualtySam[i + 4] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeCasualtySam[i + 4], 0, billboard_texcoords(i + 4, 3, false));
	}

	_billboardModel->_billboardShapeFighterSamBlue = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamBlue, 0, billboard_texcoords(1, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamBlue, 180, billboard_texcoords(2, 1, false));

	_billboardModel->_billboardShapeFighterSamRed = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamRed, 0, billboard_texcoords(1, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterSamRed, 180, billboard_texcoords(2, 2, false));

	_billboardModel->_billboardShapeFighterAshBlue = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshBlue, 0, billboard_texcoords(3, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshBlue, 180, billboard_texcoords(4, 1, false));

	_billboardModel->_billboardShapeFighterAshRed = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshRed, 0, billboard_texcoords(3, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterAshRed, 180, billboard_texcoords(4, 2, false));

	_billboardModel->_billboardShapeFighterCavBlue = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue,  45, billboard_texcoords(7, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue,  90, billboard_texcoords(6, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 135, billboard_texcoords(5, 1, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 225, billboard_texcoords(5, 1, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 270, billboard_texcoords(6, 1, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavBlue, 315, billboard_texcoords(7, 1, true));

	_billboardModel->_billboardShapeFighterCavRed = _billboardTexture->AddShape(1);
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed,  45, billboard_texcoords(7, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed,  90, billboard_texcoords(6, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 135, billboard_texcoords(5, 2, false));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 225, billboard_texcoords(5, 2, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 270, billboard_texcoords(6, 2, true));
	_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeFighterCavRed, 315, billboard_texcoords(7, 2, true));


	for (int i = 0; i < 8; ++i)
	{
		_billboardModel->_billboardShapeSmoke[i] = _billboardTexture->AddShape(1);
		_billboardTexture->SetTexCoords(_billboardModel->_billboardShapeSmoke[i], 0, billboard_texcoords(i, 7, false));
	}

	_textureBillboardRenderer = new TextureBillboardRenderer();
	_textureBillboardRenderer1 = new TextureBillboardRenderer();
	_textureBillboardRenderer2 = new TextureBillboardRenderer();

	_plainLineVertices = new VertexBuffer_3f();
	_gradientLineVertices = new VertexBuffer_3f_4f();
	_gradientTriangleVertices = new VertexBuffer_3f_4f();
	_gradientTriangleStripVertices = new VertexBuffer_3f_4f();
	_colorBillboardVertices = new VertexBuffer_3f_4f_1f();
	_textureTriangleVertices = new VertexBuffer_3f_2f();
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

	delete _textureBillboardRenderer;
	delete _textureBillboardRenderer1;
	delete _textureBillboardRenderer2;

	delete _plainLineVertices;
	delete _gradientLineVertices;
	delete _gradientTriangleVertices;
	delete _gradientTriangleStripVertices;
	delete _colorBillboardVertices;
	delete _textureTriangleVertices;

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

	GroundMap* groundMap = simulator->GetGroundMap();
	if (groundMap != nullptr)
		OnSetGroundMap(simulator->GetGroundMap());

	_simulator->AddObserver(this);
}


static bool ShouldEnableRenderEdges(float pixels_per_point)
{
#if TARGET_OS_IPHONE
	return pixels_per_point > 1;
#else
	return true;
#endif
}


void BattleView::OnSetGroundMap(GroundMap* groundMap)
{
	SmoothGroundMap* smoothGroundMap = dynamic_cast<SmoothGroundMap*>(groundMap);
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

	delete _smoothTerrainSky;
	_smoothTerrainSky = nullptr;

	delete _tiledTerrainRenderer;
	_tiledTerrainRenderer = nullptr;

	if (smoothGroundMap != nullptr)
	{
		_smoothTerrainSurface = new SmoothTerrainRenderer(GetSurface()->GetGraphicsContext(), smoothGroundMap);
		_smoothTerrainWater = new SmoothTerrainWater(smoothGroundMap);

		if (ShouldEnableRenderEdges(ShaderProgramBase::pixels_per_point()))
			_smoothTerrainSurface->EnableRenderEdges();
	}

	TiledGroundMap* tiledGroundMap = dynamic_cast<TiledGroundMap*>(groundMap);
	if (tiledGroundMap != nullptr)
	{
		tiledGroundMap->UpdateHeightMap();
		_tiledTerrainRenderer = new TiledTerrainRenderer(GetSurface()->GetGraphicsContext(), tiledGroundMap);
	}

	_smoothTerrainSky = new SmoothTerrainSky(GetSurface()->GetGraphicsContext());

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
	InitializeTerrainTrees();

	InitializeCameraPosition();
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
		glm::vec2 center = mapbounds.center();
		float radius = mapbounds.width() / 2;

		float d = 5 * mapbounds.width() / 1024;
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

	bool flip = GetFlip();

	glm::vec2 friendlyScreen = ContentToSurface(glm::vec2(0, flip ? 0.4 : -0.4));
	glm::vec2 enemyScreen = ContentToSurface(glm::vec2(0, flip ? -0.4 : 0.4));

	Zoom(GetTerrainPosition(friendlyCenter, 0), GetTerrainPosition(enemyCenter, 0), friendlyScreen, enemyScreen, 0);

	//ClampCameraPosition();
}


void BattleView::Render(const glm::mat4& transformx)
{
	glm::mat4 containerTransform = transformx * glm::inverse(GetContentTransform());
	glm::mat4 terrainTransform = GetTerrainTransform();

	glm::mat4 adjustmentTransform;
	adjustmentTransform = glm::scale(adjustmentTransform, glm::vec3(GetSize(), 1));
	adjustmentTransform = glm::scale(adjustmentTransform, glm::vec3(0.5f, 0.5f, 1));
	adjustmentTransform = glm::translate(adjustmentTransform, glm::vec3(1, 1, 0));

	glm::mat4 contentTransform = transformx * adjustmentTransform * terrainTransform;
	glm::mat4 facingTransform = transformx * glm::translate(glm::mat4(), glm::vec3(-GetFrame().min, 0));


	glm::vec2 facing = vector2_from_angle(GetCameraFacing() - 2.5f * (float)M_PI_4);
	_lightNormal = glm::normalize(glm::vec3(facing, -1));


	// Terrain Sky

	glDisable(GL_DEPTH_TEST);
	if (_smoothTerrainSky != nullptr)
	{
		_smoothTerrainSky->RenderBackgroundLinen(containerTransform, GetFrame(), GetFlip());
		_smoothTerrainSky->Render(containerTransform, GetFrame(), GetCameraDirection().z, GetFlip());
	}


	// Terrain Surface

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (_smoothTerrainSurface != nullptr)
		_smoothTerrainSurface->Render(contentTransform, _lightNormal);

	if (_tiledTerrainRenderer != nullptr)
		_tiledTerrainRenderer->Render(contentTransform, _lightNormal);


	// Terrain Water

	glDisable(GL_CULL_FACE);
	if (_smoothTerrainWater != nullptr)
		_smoothTerrainWater->Render(GetSurface()->GetGraphicsContext(), contentTransform);


	// Fighter Weapons

	glDepthMask(false);
	_plainLineVertices->Reset(GL_LINES);
	for (UnitCounter* marker : _unitMarkers)
		marker->AppendFighterWeapons(_plainLineVertices);

	glLineWidth(1);
	RenderCall<PlainShader>(GetSurface()->GetGraphicsContext())
		.SetVertices(_plainLineVertices)
		.SetUniform("transform", contentTransform)
		.SetUniform("point_size", 1)
		.SetUniform("color", glm::vec4(0.4, 0.4, 0.4, 0.6))
		.Render();


	// Color Billboards

	_colorBillboardVertices->Reset(GL_POINTS);
	_casualtyMarker->RenderCasualtyColorBillboards(_colorBillboardVertices);

	RenderCall<ColorBillboardShader>(GetSurface()->GetGraphicsContext())
		.SetVertices(_colorBillboardVertices)
		.SetUniform("transform", contentTransform)
		.SetUniform("upvector", GetCameraUpVector())
		.SetUniform("viewport_height", 0.25f * GetSurface()->GetGraphicsContext()->GetPixelDensity() * GetFrame().height())
		.Render();


	// Texture Billboards

	_billboardModel->dynamicBillboards.clear();
	_casualtyMarker->AppendCasualtyBillboards(_billboardModel);
	for (UnitCounter* marker : _unitMarkers)
		marker->AppendFighterBillboards(_billboardModel);
	for (SmokeCounter* marker : _smokeMarkers)
		marker->AppendSmokeBillboards(_billboardModel);
	_textureBillboardRenderer->Render(GetSurface()->GetGraphicsContext(), _billboardModel, contentTransform, GetCameraUpVector(), glm::degrees(GetCameraFacing()), GetFrame().height(), GetFlip());


	// Range Markers

	for (Unit* unit : _simulator->GetUnits())
	{
		if (unit->IsFriendlyCommander(_commander))
		{
			RangeMarker marker(_simulator, unit);
			_gradientTriangleStripVertices->Reset(GL_TRIANGLE_STRIP);
			marker.Render(_gradientTriangleStripVertices);

			RenderCall<GradientShader>(GetSurface()->GetGraphicsContext())
				.SetVertices(_gradientTriangleStripVertices)
				.SetUniform("transform", contentTransform)
				.SetUniform("point_size", 1)
				.Render();
		}
	}


	// Unit Facing Markers

	glDisable(GL_DEPTH_TEST);
	_textureTriangleVertices->Reset(GL_TRIANGLES);

	for (UnitCounter* marker : _unitMarkers)
		if (marker->GetUnit()->IsFriendlyCommander(_commander))
			marker->AppendFacingMarker(_textureTriangleVertices, this);
	for (UnitMovementMarker* marker : _movementMarkers)
		if (marker->GetUnit()->IsFriendlyCommander(_commander))
			marker->AppendFacingMarker(_textureTriangleVertices, this);
	for (UnitTrackingMarker* marker : _trackingMarkers)
		if (marker->GetUnit()->IsFriendlyCommander(_commander))
			marker->AppendFacingMarker(_textureTriangleVertices, this);

	RenderCall<TextureShader>(GetSurface()->GetGraphicsContext())
		.SetVertices(_textureTriangleVertices)
		.SetUniform("transform", facingTransform)
		.SetUniform("texture", _textureUnitMarkers)
		.Render();



	// Unit Markers

	_textureBillboardRenderer1->Reset();
	_textureBillboardRenderer2->Reset();

	for (UnitCounter* marker : _unitMarkers)
		marker->AppendUnitMarker(_textureBillboardRenderer2, GetFlip());
	for (UnitMovementMarker* marker : _movementMarkers)
		marker->RenderMovementMarker(_textureBillboardRenderer1);
	for (UnitTrackingMarker* marker : _trackingMarkers)
		marker->RenderTrackingMarker(_textureBillboardRenderer1);

	bounds1f sizeLimit = GetUnitIconSizeLimit();
	_textureBillboardRenderer1->Draw(GetSurface()->GetGraphicsContext(), _textureUnitMarkers, contentTransform, GetCameraUpVector(), glm::degrees(GetCameraFacing()), GetFrame().height(), sizeLimit);
	_textureBillboardRenderer2->Draw(GetSurface()->GetGraphicsContext(), _textureUnitMarkers, contentTransform, GetCameraUpVector(), glm::degrees(GetCameraFacing()), GetFrame().height(), sizeLimit);


	// Tracking Markers

	glDisable(GL_DEPTH_TEST);
	for (UnitTrackingMarker* marker : _trackingMarkers)
	{
		_textureBillboardRenderer1->Reset();
		marker->RenderTrackingShadow(_textureBillboardRenderer1);
		_textureBillboardRenderer1->Draw(GetSurface()->GetGraphicsContext(), _textureTouchMarker, contentTransform, GetCameraUpVector(), glm::degrees(GetCameraFacing()), GetFrame().height(), bounds1f(64, 64));
	}


	// Movement Paths

	glEnable(GL_DEPTH_TEST);
	_gradientTriangleVertices->Reset(GL_TRIANGLES);
	for (UnitMovementMarker* marker : _movementMarkers)
		marker->RenderMovementPath(_gradientTriangleVertices);

	RenderCall<GradientShader>(GetSurface()->GetGraphicsContext())
		.SetVertices(_gradientTriangleVertices)
		.SetUniform("transform", contentTransform)
		.SetUniform("point_size", 1)
		.Render();


	// Tracking Path

	glDisable(GL_DEPTH_TEST);
	for (UnitTrackingMarker* marker : _trackingMarkers)
	{
		_gradientTriangleVertices->Reset(GL_TRIANGLES);
		marker->RenderTrackingPath(_gradientTriangleVertices);
		marker->RenderOrientation(_gradientTriangleVertices);

		RenderCall<GradientShader>(GetSurface()->GetGraphicsContext())
			.SetVertices(_gradientTriangleVertices)
			.SetUniform("transform", contentTransform)
			.SetUniform("point_size", 1)
			.Render();
	}


	// Tracking Fighters

	glEnable(GL_DEPTH_TEST);
	_colorBillboardVertices->Reset(GL_POINTS);
	for (UnitTrackingMarker* marker : _trackingMarkers)
		marker->RenderTrackingFighters(_colorBillboardVertices);

	RenderCall<ColorBillboardShader>(GetSurface()->GetGraphicsContext())
		.SetVertices(_colorBillboardVertices)
		.SetUniform("transform", contentTransform)
		.SetUniform("upvector", GetCameraUpVector())
		.SetUniform("viewport_height", 0.25f * GetSurface()->GetGraphicsContext()->GetPixelDensity() * GetFrame().height())
		.Render();


	// Movement Fighters

	_colorBillboardVertices->Reset(GL_POINTS);
	for (UnitMovementMarker* marker : _movementMarkers)
		marker->RenderMovementFighters(_colorBillboardVertices);

	RenderCall<ColorBillboardShader>(GetSurface()->GetGraphicsContext())
		.SetVertices(_colorBillboardVertices)
		.SetUniform("transform", contentTransform)
		.SetUniform("upvector", GetCameraUpVector())
		.SetUniform("viewport_height", 0.25f * GetSurface()->GetGraphicsContext()->GetPixelDensity() * GetFrame().height())
		.Render();


	// Shooting Counters

	_gradientLineVertices->Reset(GL_LINES);
	for (ShootingCounter* shootingCounter : _shootingCounters)
		shootingCounter->Render(_gradientLineVertices);

	RenderCall<GradientShader>(GetSurface()->GetGraphicsContext())
		.SetVertices(_gradientLineVertices)
		.SetUniform("transform", contentTransform)
		.SetUniform("point_size", 1)
		.Render();


	// Mouse Hint

	_plainLineVertices->Reset(GL_LINES);
	RenderMouseHint(_plainLineVertices);

	glLineWidth(1);
	RenderCall<PlainShader>(GetSurface()->GetGraphicsContext())
		.SetVertices(_plainLineVertices)
		.SetUniform("transform", contentTransform)
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




void BattleView::Update(double secondsSinceLastUpdate)
{
	_casualtyMarker->Animate((float)secondsSinceLastUpdate);

	::AnimateMarkers(_movementMarkers, (float)secondsSinceLastUpdate);
}


void BattleView::FindHotspots(const glm::mat4 transform, glm::vec2 position, Touch* touch)
{

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
	glm::mat4x4 transform = GetTerrainTransform();
	glm::vec3 upvector = GetCameraUpVector();
	float viewport_height = GetFrame().height();
	bounds1f sizeLimit = GetUnitIconSizeLimit() / ShaderProgramBase::pixels_per_point();

	glm::vec3 position2 = position + height * 0.5f * viewport_height * upvector;
	glm::vec4 p = transform * glm::vec4(position, 1);
	glm::vec4 q = transform * glm::vec4(position2, 1);
	float s = glm::clamp(glm::abs(q.y / q.w - p.y / p.w), sizeLimit.min, sizeLimit.max);

	return bounds2_from_center(ContentToSurface((glm::vec2)p.xy() / p.w), s / 2);
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

	glm::vec2 position = iconBounds.center();
	float size = iconBounds.height();
	float adjust = GetFlip() ? 3 * glm::half_pi<float>() : glm::half_pi<float>();

	position += 0.7f * size * vector2_from_angle(direction - GetCameraFacing() + adjust);

	return bounds2_from_center(position, 0.2f * size);
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
	float y = GetCameraDirection().z;
	float x = sqrtf(1 - y * y);
	float a = 1 - fabsf(atan2f(y, x) / (float)M_PI_2);

	bounds1f result(0, 0);
	result.min = (32 - 8 * a) * ShaderProgramBase::pixels_per_point();
	result.max = (32 + 16 * a) * ShaderProgramBase::pixels_per_point();
	if (is_iphone())
	{
		result.min *= 57.0f / 72.0f;
		result.max *= 57.0f / 72.0f;
	}
	return result;
}


/***/




void BattleView::AnimateMarkers(float seconds)
{
	::AnimateMarkers(_unitMarkers, seconds);
	::AnimateMarkers(_shootingCounters, seconds);
	::AnimateMarkers(_smokeMarkers, seconds);
}


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
