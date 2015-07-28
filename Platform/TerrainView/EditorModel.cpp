// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "EditorModel.h"
#include "BattleModel/BattleSimulator_v1_0_0.h"
#include "BattleView/BattleView.h"
#include "SmoothTerrain/SmoothTerrainWater.h"



EditorModelObserver::~EditorModelObserver()
{

}


EditorModel::EditorModel(BattleView* battleView, SmoothTerrainRenderer* smoothTerrainSurface) :
_battleView(battleView),
_smoothTerrainRenderer(smoothTerrainSurface),
_editorMode(EditorMode::Hand),
_terrainFeature(TerrainFeature::Hills),
_brush(nullptr)
{
	_brush = new Image(48, 48);
	_mixer = new Image(48, 48);
}


void EditorModel::AddObserver(EditorModelObserver* observer)
{
	_observers.insert(observer);
}


void EditorModel::RemoveObserver(EditorModelObserver* observer)
{
	_observers.erase(observer);
}


MapEditor* EditorModel::GetMapEditor() const
{
	return _smoothTerrainRenderer->GetSmoothGroundMap();
}


EditorMode EditorModel::GetEditorMode() const
{
	return _editorMode;
}


void EditorModel::SetEditorMode(EditorMode value)
{
	if (value != _editorMode)
	{
		_editorMode = value;
		for (EditorModelObserver* observer : _observers)
			observer->OnEditorModeChanged(this);
	}
}


TerrainFeature EditorModel::GetTerrainFeature() const
{
	return _terrainFeature;
}


void EditorModel::SetTerrainFeature(TerrainFeature value)
{
	if (value != _terrainFeature)
	{
		_terrainFeature = value;
		for (EditorModelObserver* observer : _observers)
			observer->OnTerrainFeatureChanged(this);
	}
}


void EditorModel::ToolBegan(glm::vec2 position)
{
	switch (_editorMode)
	{
		case EditorMode::Smear:
			SmearReset(_terrainFeature, position);
			break;

		case EditorMode::Paint:
			Paint(_terrainFeature, position, 1);
			break;

		case EditorMode::Erase:
			Paint(_terrainFeature, position, 0);
			break;

		default:
			break;
	}
}


void EditorModel::ToolMoved(glm::vec2 position)
{
	switch (_editorMode)
	{
		case EditorMode::Smear:
			SmearPaint(_terrainFeature, position);
			break;

		case EditorMode::Paint:
			Paint(_terrainFeature, position, 1);
			break;

		case EditorMode::Erase:
			Paint(_terrainFeature, position, 0);
			break;

		default:
			break;
	}
}


void EditorModel::ToolEnded(glm::vec2 position)
{
	switch (_editorMode)
	{
		case EditorMode::Smear:
			SmearPaint(_terrainFeature, position);
			break;

		case EditorMode::Paint:
			Paint(_terrainFeature, position, 1);
			break;

		case EditorMode::Erase:
			Paint(_terrainFeature, position, 0);
			break;

		default:
			break;
	}
}


void EditorModel::Paint(TerrainFeature feature, glm::vec2 position, bool value)
{
	float pressure = value ? 0.4f : -0.4f;
	float radius = feature == TerrainFeature::Hills ? 48 : 16;
	bounds2f bounds = GetMapEditor()->Paint(feature, position, pressure, radius);

	_smoothTerrainRenderer->UpdateChanges(bounds);

	if (feature != TerrainFeature::Fords)
		_battleView->UpdateTerrainTrees(bounds);

	if (feature != TerrainFeature::Trees)
		_battleView->GetSmoothTerrainWater()->Update();
}


void EditorModel::SmearReset(TerrainFeature feature, glm::vec2 position)
{
	GetMapEditor()->Extract(position, *_brush);
	_brushPosition = position;
	_brushDistance = 0;
}


void EditorModel::SmearPaint(TerrainFeature feature, glm::vec2 position)
{
	_brushDistance += glm::distance(_brushPosition, position);
	_brushPosition = position;
	while (_brushDistance > 2.0f)
	{
		GetMapEditor()->Extract(position, *_mixer);

		glm::ivec2 size = _brush->size();
		for (int x = 0; x < size.x; ++x)
			for (int y = 0; y < size.y; ++y)
			{
				glm::vec4 c = _brush->GetPixel(x, y);
				glm::vec4 m = _mixer->GetPixel(x, y);
				c = glm::mix(c, m, 0.1f);
				_brush->SetPixel(x, y, c);
			}

		_brushDistance -= 2.0f;
	}

	bounds2f bounds = GetMapEditor()->Paint(feature, position, 0.5f, *_brush);

	_smoothTerrainRenderer->UpdateChanges(bounds);

	if (feature != TerrainFeature::Fords)
		_battleView->UpdateTerrainTrees(bounds);

	if (feature != TerrainFeature::Trees)
		_battleView->GetSmoothTerrainWater()->Update();
}
