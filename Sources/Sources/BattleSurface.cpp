#include "../BattleModel/BattleScenario.h"
#include "../BattleModel/BattleSimulator.h"
#include "../Library/Audio/SoundPlayer.h"
#include "BattleView/BattleGesture.h"
#include "BattleView/BattleView.h"
#include "BattleView/UnitCounter.h"
#include "SmoothTerrain/SmoothTerrainWater.h"
#include "SmoothTerrain/SmoothTerrainSky.h"
#include "TerrainView/TerrainGesture.h"
#include "BattleSurface.h"


BattleSurface::BattleSurface(glm::vec2 size, float pixelDensity) : Surface(size, pixelDensity),
_renderers(nullptr),
_playing(false),
_editing(false),
_scenario(nullptr),
_battleView(nullptr),
_battleGesture(nullptr),
_terrainGesture(nullptr)
{
	_renderers = renderers::singleton = new renderers();

	SoundPlayer::Initialize();
	SoundPlayer::singleton->Pause();

}


BattleSurface::~BattleSurface()
{
}


void BattleSurface::Reset(BattleScenario* scenario)
{
	delete _terrainGesture;
	_terrainGesture = nullptr;

	delete _battleGesture;
	_battleGesture = nullptr;

	if (_battleView != nullptr)
	{
		delete _battleView->_smoothTerrainSurface;
		_battleView->_smoothTerrainSurface = nullptr;

		delete _battleView->_tiledTerrainRenderer;
		_battleView->_tiledTerrainRenderer = nullptr;
	}

	delete _battleView;
	_battleView = nullptr;

	delete _scenario;
	_scenario = nullptr;



	_scenario = scenario;

	_battleView = new BattleView(this, scenario->GetSimulator(), _renderers);
	_battleView->_player = 1;
	_battleView->_blueTeam = 1;

	SmoothGroundMap* smoothGroundMap = dynamic_cast<SmoothGroundMap*>(scenario->GetSimulator()->GetGroundMap());
	if (smoothGroundMap != nullptr)
	{
		_battleView->_smoothTerrainSurface = new SmoothTerrainRenderer(smoothGroundMap);
		_battleView->_smoothTerrainSurface->EnableRenderEdges();
		_battleView->_smoothTerrainWater = new SmoothTerrainWater(smoothGroundMap);
		_battleView->_smoothTerrainSky = new SmoothTerrainSky();
	}

	TiledGroundMap* tiledGroundMap = dynamic_cast<TiledGroundMap*>(scenario->GetSimulator()->GetGroundMap());
	if (tiledGroundMap != nullptr)
	{
		tiledGroundMap->UpdateHeightMap();
		_battleView->_tiledTerrainRenderer = new TiledTerrainRenderer(tiledGroundMap);
		_battleView->_smoothTerrainSky = new SmoothTerrainSky();
	}

	_battleView->Initialize();

	_battleGesture = new BattleGesture(_battleView);
	_terrainGesture = new TerrainGesture(_battleView);
	scenario->GetSimulator()->AddObserver(_battleView);
}


void BattleSurface::SetPlaying(bool value)
{
	_playing = value;

	if (_playing)
		SoundPlayer::singleton->Resume();
	else
		SoundPlayer::singleton->Pause();

	if (_battleGesture != nullptr)
		_battleGesture->SetEnabled(_playing);
}


void BattleSurface::SetEditing(bool value)
{
	_editing = value;

	if (_terrainGesture != nullptr)
		_terrainGesture->SetEnabled(!_editing);
}


void BattleSurface::ScreenSizeChanged()
{
	Surface::ScreenSizeChanged();

	if (_battleView != nullptr)
	{
		bounds2f viewport = bounds2f(0, 0, GetSize());
		_battleView->SetViewport(viewport);
	}
}


void BattleSurface::Update(double secondsSinceLastUpdate)
{
	if (_playing)
	{
		_scenario->Tick(secondsSinceLastUpdate);
		UpdateSoundPlayer();
	}
	else if (_scenario != nullptr)
	{
		_scenario->Tick(0);
	}

	if (_battleView != nullptr)
	{
		_battleView->Update(secondsSinceLastUpdate);
		_battleView->AnimateMarkers((float)secondsSinceLastUpdate);
	}
}


bool BattleSurface::NeedsRender() const
{
	return true;
}


void BattleSurface::Render()
{
	if (_battleView != nullptr)
	{
		_battleView->Render();

		/*
		if (_battleScript != nullptr)
		{
			_scriptHintRenderer->Reset();
			_battleScript->RenderHints(_scriptHintRenderer);
			_scriptHintRenderer->Draw(_battleView->GetTransform());
		}
		*/
	}

	//if (_battleGesture != nullptr)
	//	_battleGesture->RenderHints();
}


void BattleSurface::UpdateSoundPlayer()
{
	if (_playing)
	{
		int horseGallop = 0;
		int horseTrot = 0;
		int fighting = 0;
		int infantryMarching = 0;
		int infantryRunning = 0;

		for (UnitCounter* unitMarker : _battleView->GetUnitCounters())
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

		SoundPlayer::singleton->UpdateFighting(_scenario->GetSimulator()->IsMelee());
	}
}
