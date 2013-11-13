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
_scenario(nullptr)
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
	RemoveBattleViews();

	delete _scenario;
	_scenario = scenario;

	CreateBattleViews();
}


void BattleSurface::SetPlaying(bool value)
{
	_playing = value;

	if (_playing)
		SoundPlayer::singleton->Resume();
	else
		SoundPlayer::singleton->Pause();

	for (BattleGesture* gesture : _battleGestures)
		gesture->SetEnabled(_playing);
}


void BattleSurface::SetEditing(bool value)
{
	_editing = value;

	for (TerrainGesture* gesture : _terrainGestures)
		gesture->SetEnabled(!_editing);
}


void BattleSurface::ScreenSizeChanged()
{
	Surface::ScreenSizeChanged();
	UpdateBattleViewSize();
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

	for (BattleView* battleView : _battleViews)
	{
		battleView->Update(secondsSinceLastUpdate);
		battleView->AnimateMarkers((float)secondsSinceLastUpdate);
	}
}


bool BattleSurface::NeedsRender() const
{
	return true;
}


void BattleSurface::Render()
{
	for (BattleView* battleView : _battleViews)
	{
		battleView->Render();

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


void BattleSurface::RemoveBattleViews()
{
	for (Gesture* gesture : _battleGestures)
		delete gesture;
	_battleGestures.clear();

	for (BattleView* battleView : _battleViews)
	{
		delete battleView->_smoothTerrainSurface;
		battleView->_smoothTerrainSurface = nullptr;

		delete battleView->_tiledTerrainRenderer;
		battleView->_tiledTerrainRenderer = nullptr;

		delete battleView;
	}
	_battleViews.clear();
}


static bool ShouldEnableRenderEdges(float pixels_per_point)
{
#if TARGET_OS_IPHONE
	return pixels_per_point > 1;
#else
	return true;
#endif
}


void BattleSurface::CreateBattleViews()
{
	BattleSimulator* simulator = _scenario->GetSimulator();

	SmoothTerrainRenderer* smoothTerrainRenderer = nullptr;
	SmoothTerrainWater* smoothTerrainWater = nullptr;
	SmoothTerrainSky* smoothTerrainSky = nullptr;
	SmoothGroundMap* smoothGroundMap = dynamic_cast<SmoothGroundMap*>(simulator->GetGroundMap());
	if (smoothGroundMap != nullptr)
	{
		smoothTerrainRenderer = new SmoothTerrainRenderer(smoothGroundMap);
		smoothTerrainWater = new SmoothTerrainWater(smoothGroundMap);
		smoothTerrainSky = new SmoothTerrainSky();

		if (ShouldEnableRenderEdges(renderer_base::pixels_per_point()))
			smoothTerrainRenderer->EnableRenderEdges();
	}

	TiledTerrainRenderer* tiledTerrainRenderer = nullptr;
	TiledGroundMap* tiledGroundMap = dynamic_cast<TiledGroundMap*>(simulator->GetGroundMap());
	if (tiledGroundMap != nullptr)
	{
		tiledGroundMap->UpdateHeightMap();
		tiledTerrainRenderer = new TiledTerrainRenderer(tiledGroundMap);
		smoothTerrainSky = new SmoothTerrainSky();
	}

	std::vector<BattleCommander*> commanders;

	for (BattleCommander* commander : _scenario->GetCommanders())
		if (commander->GetType() == BattleCommanderType::Screen)
			commanders.push_back(commander);

	if (commanders.empty())
		commanders.push_back(new BattleCommander(0, BattleCommanderType::None, ""));

	for (BattleCommander* commander : commanders)
	{
		BattleView* battleView = new BattleView(this, simulator, _renderers);
		battleView->SetCommander(commander);
		battleView->_smoothTerrainSurface = smoothTerrainRenderer;
		battleView->_smoothTerrainWater = smoothTerrainWater;
		battleView->_smoothTerrainSky = smoothTerrainSky;
		battleView->_tiledTerrainRenderer = tiledTerrainRenderer;

		if (commander->GetConfiguration()[0] == '-')
		{
			battleView->SetFlip(true);
			battleView->SetCameraFacing((float)M_PI);
		}

		battleView->Initialize();
		_battleViews.push_back(battleView);
		simulator->AddObserver(battleView);

		BattleGesture* battleGesture = new BattleGesture(battleView);
		_battleGestures.push_back(battleGesture);

		TerrainGesture* terrainGesture = new TerrainGesture(battleView);
		_terrainGestures.push_back(terrainGesture);
	}

	UpdateBattleViewSize();
}


void BattleSurface::UpdateSoundPlayer()
{
	if (_playing && !_battleViews.empty())
	{
		int horseGallop = 0;
		int horseTrot = 0;
		int fighting = 0;
		int infantryMarching = 0;
		int infantryRunning = 0;

		for (UnitCounter* unitMarker : _battleViews.front()->GetUnitCounters())
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


void BattleSurface::UpdateBattleViewSize()
{
	if (!_battleViews.empty())
	{
		glm::vec2 size = GetSize();
		float h = size.y / _battleViews.size();
		float y = 0;
		for (BattleView* battleView : _battleViews)
		{
			battleView->SetViewport(bounds2f(0, y, size.x, y + h));
			y += h;
		}
	}
}
