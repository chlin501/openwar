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


void BattleSurface::ResetBattleViews(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders)
{
	_scenario = scenario;
	_commanders = commanders;

	int count = scenario != nullptr ? (int)commanders.size() : 0;

	while ((int)_battleViews.size() > count)
		RemoveBattleView(_battleViews.back());

	for (int i = 0; i < count; ++i)
	{
		BattleCommander* commander = _commanders[i];
		if (i < (int)_battleViews.size())
			ResetBattleView(_battleViews[i], commander);
		else
			CreateBattleView(commander);
	}

	UpdateBattleViewSize();
}


void BattleSurface::ResetCameraPosition()
{
	for (BattleView* battleView : _battleViews)
		battleView->InitializeCameraPosition();
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



void BattleSurface::CreateBattleView(BattleCommander* commander)
{
	BattleSimulator* simulator = _scenario->GetSimulator();

	BattleView* battleView = new BattleView(this, _renderers);
	battleView->SetCommander(commander);
	battleView->SetSimulator(simulator);

	if (commander->GetConfiguration()[0] == '-')
	{
		battleView->SetFlip(true);
		battleView->SetCameraFacing((float)M_PI);
	}

	battleView->Initialize();
	_battleViews.push_back(battleView);

	BattleGesture* battleGesture = new BattleGesture(battleView);
	_battleGestures.push_back(battleGesture);

	TerrainGesture* terrainGesture = new TerrainGesture(battleView);
	_terrainGestures.push_back(terrainGesture);
}


void BattleSurface::ResetBattleView(BattleView* battleView, BattleCommander* commander)
{
	battleView->SetCommander(commander);
	battleView->SetSimulator(_scenario->GetSimulator());
}


void BattleSurface::RemoveBattleView(BattleView* battleView)
{
	for (auto i = _battleGestures.begin(); i != _battleGestures.end(); )
	{
		if ((*i)->GetBattleView() == battleView)
		{
			delete *i;
			i = _battleGestures.erase(i);
		}
		else
		{
			++i;
		}
	}

	for (auto i = _terrainGestures.begin(); i != _terrainGestures.end(); )
	{
		if ((*i)->GetTerrainView() == battleView)
		{
			delete *i;
			i = _terrainGestures.erase(i);
		}
		else
		{
			++i;
		}
	}

	for (auto i = _battleViews.begin(); i != _battleViews.end(); )
	{
		if (*i == battleView)
			i = _battleViews.erase(i);
		else
			++i;
	}

	battleView->GetSimulator()->RemoveObserver(battleView);
	delete battleView;
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
