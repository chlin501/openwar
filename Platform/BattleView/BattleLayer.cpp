#include "BattleModel/BattleScenario.h"
#include "BattleModel/BattleSimulator.h"
#include "Audio/SoundPlayer.h"
#include "Surface/Surface.h"
#include "BattleHotspot.h"
#include "BattleGesture.h"
#include "BattleView.h"
#include "BattleLayer.h"
#include "EditorHotspot.h"
#include "UnitCounter.h"
#include "SmoothTerrain/SmoothTerrainWater.h"
#include "SmoothTerrain/SmoothTerrainSky.h"
#include "TerrainView/EditorGesture.h"
#include "TerrainView/EditorHotspot.h"
#include "TerrainView/EditorModel.h"
#include "TerrainView/TerrainGesture.h"
#include "TerrainView/TerrainHotspot.h"

#include <glm/gtc/matrix_transform.hpp>


BattleLayer::BattleLayer(GraphicsContext* gc) : Container(gc),
_playing(false),
_editing(false),
_scenario(nullptr),
_editorModel(nullptr)
{
	SoundPlayer::Initialize();
	SoundPlayer::singleton->Pause();
}


BattleLayer::~BattleLayer()
{
}


void BattleLayer::ResetBattleViews(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders)
{
	delete _editorModel;
	_editorModel = nullptr;
	_editorHotspot = nullptr;

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


void BattleLayer::ResetEditor(BattleScenario* scenario)
{
	delete _editorModel;
	_editorModel = nullptr;
	_editorHotspot = nullptr;

	_scenario = scenario;
	_commanders.clear();

	while ((int)_battleViews.size() > 1)
		RemoveBattleView(_battleViews.back());

	if (_battleViews.empty())
		CreateBattleView(nullptr);
	else
		ResetBattleView(_battleViews.front(), nullptr);

	_editorModel = new EditorModel(_battleViews.front(), _battleViews.front()->GetSmoothTerrainRenderer());
	_editorHotspot = std::make_shared<EditorHotspot>(_battleViews.front(), _editorModel);
	_editorModel->AddObserver(this);

	UpdateBattleViewSize();
}


void BattleLayer::ResetCameraPosition()
{
	for (BattleView* battleView : _battleViews)
		battleView->InitializeCameraPosition();
}


void BattleLayer::SetPlaying(bool value)
{
	_playing = value;

	if (_playing)
		SoundPlayer::singleton->Resume();
	else
		SoundPlayer::singleton->Pause();

	//for (BattleGesture* gesture : _battleGestures)
	//	gesture->SetEnabled(_playing);
}


void BattleLayer::SetEditing(bool value)
{
	_editing = value;

	//for (TerrainGesture* gesture : _terrainGestures)
	//	gesture->SetEnabled(!_editing);
}


/* EditorModelObserver */

void BattleLayer::OnEditorModeChanged(EditorModel* editorModel)
{
	SetEditing(editorModel->GetEditorMode() != EditorMode::Hand);
}


void BattleLayer::OnTerrainFeatureChanged(EditorModel* editorModel)
{
}


/***/


void BattleLayer::Update(double secondsSinceLastUpdate)
{
	UpdateBattleViewSize();

	if (_scenario != nullptr)
	{
		if (_playing)
			_scenario->Tick(secondsSinceLastUpdate);
		else
			_scenario->Tick(0);

		if (_playing)
			UpdateSoundPlayer();
	}

	Container::Update(secondsSinceLastUpdate);

	for (BattleView* battleView : _battleViews)
	{
		battleView->AnimateMarkers((float)secondsSinceLastUpdate);
	}
}


void BattleLayer::FindHotspots(Touch* touch)
{
	if (_editorHotspot != nullptr)
		touch->AddHotspot(_editorHotspot);

	Container::FindHotspots(touch);
}


void BattleLayer::CreateBattleView(BattleCommander* commander)
{
	BattleSimulator* simulator = _scenario->GetSimulator();

	BattleView* battleView = new BattleView(GetGraphicsContext());
	battleView->SetContainer(this);

	_battleViews.push_back(battleView);
	UpdateBattleViewSize();

	battleView->SetFlip(commander != _commanders[0]);
	battleView->SetCommander(commander);
	battleView->SetSimulator(simulator);

	battleView->Initialize();

	//BattleGesture* battleGesture = new BattleGesture(new BattleHotspot(battleView));
	//_battleGestures.push_back(battleGesture);


	//TerrainGesture* terrainGesture = new TerrainGesture(new TerrainHotspot(battleView));
	//_terrainGestures.push_back(terrainGesture);
}


void BattleLayer::ResetBattleView(BattleView* battleView, BattleCommander* commander)
{
	battleView->SetFlip(commander != _commanders[0]);
	battleView->SetCommander(commander);
	battleView->SetSimulator(_scenario->GetSimulator());

	//for (auto i = _battleGestures.begin(); i != _battleGestures.end(); ++i)
	//	if ((*i)->GetBattleView() == battleView)
	//	{
	//		delete *i;
	//		*i = new BattleGesture(new BattleHotspot(battleView));
	//	}

	//for (auto i = _terrainGestures.begin(); i != _terrainGestures.end(); ++i)
	//	if ((*i)->GetTerrainView() == battleView)
	//	{
	//		delete *i;
	//		*i = new TerrainGesture(new TerrainHotspot(battleView));
	//	}
}


void BattleLayer::RemoveBattleView(BattleView* battleView)
{
	//for (auto i = _battleGestures.begin(); i != _battleGestures.end(); )
	//{
	//	if ((*i)->GetBattleView() == battleView)
	//	{
	//		delete *i;
	//		i = _battleGestures.erase(i);
	//	}
	//	else
	//	{
	//		++i;
	//	}
	//}

	//for (auto i = _terrainGestures.begin(); i != _terrainGestures.end(); )
	//{
	//	if ((*i)->GetTerrainView() == battleView)
	//	{
	//		delete *i;
	//		i = _terrainGestures.erase(i);
	//	}
	//	else
	//	{
	//		++i;
	//	}
	//}

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


void BattleLayer::UpdateBattleViewSize()
{
	if (!_battleViews.empty())
	{
		float count = _battleViews.size();

		bounds1i viewport_x = GetViewportBounds().x();
		bounds1f viewport_y = GetViewportBounds().y();

		bounds1f content_x = GetContentBounds().x();
		float content_y = GetContentBounds().y().size() / count;

		int index = 0;
		for (BattleView* battleView : _battleViews)
		{
			float k1 = index / count;
			float k2 = (index + 1) / count;

			battleView->SetViewportBounds(bounds2i(viewport_x, (int)viewport_y.mix(k1), (int)viewport_y.mix(k2)));
			battleView->SetContentBounds(bounds2f(content_x, 0, content_y));

			++index;
		}
	}
}


void BattleLayer::UpdateSoundPlayer()
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
