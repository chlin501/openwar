#include "BattleSimulator_v1_0_0.h"
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
	_surface{surface},
	_gc{surface->GetGraphicsContext()}
{
}


BattleLayer::~BattleLayer()
{
}


void BattleLayer::ResetBattleViews(BattleSimulator_v1_0_0* scenario, const std::vector<BattleObjects::Commander*>& commanders)
{
	delete _editorModel;
	_editorModel = nullptr;
	_editorHotspot = nullptr;

	_scenario = scenario;
	_commanders = commanders;

	int count = scenario ? (int)commanders.size() : 0;

	while ((int)_battleViews.size() > count)
		RemoveBattleView(_battleViews.back());

	for (int i = 0; i < count; ++i)
	{
		BattleObjects::Commander* commander = _commanders[i];
		if (i < (int)_battleViews.size())
			ResetBattleView(_battleViews[i], commander);
		else
			CreateBattleView(commander);
	}

	UpdateBattleViewSize();
}


void BattleLayer::ResetEditor(BattleSimulator_v1_0_0* scenario, const std::vector<BattleObjects::Commander*>& commanders)
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


void BattleLayer::Animate(double secondsSinceLastUpdate)
{
	UpdateBattleViewSize();

	if (_scenario)
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


void BattleLayer::CreateBattleView(BattleObjects::Commander* commander)
{
	BattleSimulator_v1_0_0* simulator = _scenario;

	std::shared_ptr<TerrainViewport> viewport = std::make_shared<TerrainViewport>(_gc);

	BattleView* battleView = new BattleView(_surface, viewport);
	battleView->OrderBack();

	_battleViews.push_back(battleView);
	UpdateBattleViewSize();

	battleView->GetTerrainViewport().SetFlip(commander != _commanders[0]);
	battleView->SetCommander(commander);
	battleView->SetSimulator(simulator);

	battleView->Initialize();
}


void BattleLayer::ResetBattleView(BattleView* battleView, BattleObjects::Commander* commander)
{
	battleView->GetTerrainViewport().SetFlip(commander != _commanders[0]);
	battleView->SetCommander(commander);
	battleView->SetSimulator(_scenario);
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

	//battleView->GetSimulator()->RemoveObserver(battleView);
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

			battleView->GetViewport().SetViewportBounds(viewport_bounds);

			++index;
		}
	}
}
