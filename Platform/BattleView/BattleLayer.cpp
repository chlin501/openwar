#include "BattleModel/BattleScenario.h"
#include "BattleModel/BattleSimulator.h"
#include "Surface/Surface.h"
#include "BattleHotspot.h"
#include "BattleView.h"
#include "BattleLayer.h"
#include "TerrainView/EditorHotspot.h"
#include "UnitCounter.h"
#include "SmoothTerrain/SmoothTerrainWater.h"
#include "TerrainView/TerrainGesture.h"
#include "TerrainView/TerrainViewport.h"

#include <glm/gtc/matrix_transform.hpp>


BattleLayer::BattleLayer(Surface* surface) :
	_surface(surface),
	_gc(surface->GetGraphicsContext()),
	_playing(false),
	_editing(false),
	_scenario(nullptr),
	_editorModel(nullptr)
{
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
	}
}


void BattleLayer::Render()
{
	for (BattleView* battleView : _battleViews)
	{
		battleView->Render();
	}
}


void BattleLayer::CreateBattleView(BattleCommander* commander)
{
	BattleSimulator* simulator = _scenario->GetSimulator();

	BattleView* battleView = new BattleView(_surface);
	battleView->OrderBack();

	_battleViews.push_back(battleView);
	UpdateBattleViewSize();

	battleView->GetTerrainViewport()->SetFlip(commander != _commanders[0]);
	battleView->SetCommander(commander);
	battleView->SetSimulator(simulator);

	battleView->Initialize();
}


void BattleLayer::ResetBattleView(BattleView* battleView, BattleCommander* commander)
{
	battleView->GetTerrainViewport()->SetFlip(commander != _commanders[0]);
	battleView->SetCommander(commander);
	battleView->SetSimulator(_scenario->GetSimulator());
}


void BattleLayer::RemoveBattleView(BattleView* battleView)
{
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

		glm::vec2 surface_size = _surface->GetVirtualSize();
		bounds1f viewport_x = bounds1f(0, surface_size.x);
		bounds1f viewport_y = bounds1f(0, surface_size.y);

		int index = 0;
		for (BattleView* battleView : _battleViews)
		{
			float k1 = index / count;
			float k2 = (index + 1) / count;

			bounds2f viewport_bounds(viewport_x, viewport_y.mix(k1), viewport_y.mix(k2));

			battleView->SetBounds(viewport_bounds);

			++index;
		}
	}
}
