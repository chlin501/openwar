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
#include "TerrainView/TerrainViewport.h"

#include <glm/gtc/matrix_transform.hpp>


BattleLayer::BattleLayer(GraphicsContext* gc) : Container(gc),
	_gc(gc),
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


bounds2i BattleLayer::GetViewportBounds() const
{
	return _viewportBounds;
}


void BattleLayer::SetViewportBounds(const bounds2i& value)
{
	_viewportBounds = value;
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


void BattleLayer::ResetEditor(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders)
{
	delete _editorModel;
	_editorModel = nullptr;
	_editorHotspot = nullptr;

	_scenario = scenario;
	_commanders = commanders;

	while ((int)_battleViews.size() > 1)
		RemoveBattleView(_battleViews.back());

	if (_battleViews.empty())
		CreateBattleView(commanders.front());
	else
		ResetBattleView(_battleViews.front(), commanders.front());

	_editorModel = new EditorModel(_battleViews.front(), _battleViews.front()->GetSmoothTerrainRenderer());
	_editorHotspot = std::make_shared<EditorHotspot>(_battleViews.front(), _editorModel);
	_battleViews.front()->SetEditorHotspot(_editorHotspot);
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


void BattleLayer::OnRenderLoop(double secondsSinceLastUpdate)
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

	for (BattleView* battleView : _battleViews)
	{
		battleView->AnimateMarkers((float)secondsSinceLastUpdate);
	}
}


void BattleLayer::Render()
{
	for (BattleView* battleView : _battleViews)
	{
		battleView->Render();
	}
}


bool BattleLayer::HasChangedSinceLastRender() const
{
	return false;
}


void BattleLayer::CreateBattleView(BattleCommander* commander)
{
	BattleSimulator* simulator = _scenario->GetSimulator();

	BattleView* battleView = new BattleView(_gc);

	_battleViews.push_back(battleView);
	UpdateBattleViewSize();

	battleView->GetTerrainViewport()->SetFlip(commander != _commanders[0]);
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
	battleView->GetTerrainViewport()->SetFlip(commander != _commanders[0]);
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

		bounds1i viewport_x = _viewportBounds.x();
		bounds1f viewport_y = _viewportBounds.y();

		int index = 0;
		for (BattleView* battleView : _battleViews)
		{
			float k1 = index / count;
			float k2 = (index + 1) / count;

			bounds2i viewport_bounds(viewport_x, (int)viewport_y.mix(k1), (int)viewport_y.mix(k2));

			battleView->GetTerrainViewport()->SetBounds(viewport_bounds);
			battleView->GetWidgetViewport()->SetBounds(viewport_bounds);

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
