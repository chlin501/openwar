// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "EditorGesture.h"
#include "BattleView/BattleView.h"
#include "Surface/Touch.h"
#include "EditorModel.h"
#include "EditorHotspot.h"



EditorGesture::EditorGesture(EditorHotspot* hotspot) :
	_hotspot(hotspot)
{
}


void EditorGesture::Update(double secondsSinceLastUpdate)
{

}


void EditorGesture::TouchCaptured(Touch* touch)
{
}


void EditorGesture::TouchReleased(Touch* touch)
{
}


void EditorGesture::TouchBegan(Touch* touch)
{
	if (touch->IsCaptured() || _hotspot->HasCapturedTouch())
		return;

	if (_hotspot->GetEditorModel()->GetEditorMode() == EditorMode::Hand)
		return;

	if (_hotspot->TryCaptureTouch(touch))
	{
		_hotspot->GetEditorModel()->ToolBegan(TerrainPosition(touch));
	}
}


void EditorGesture::TouchMoved()
{
	if (_hotspot->HasCapturedTouch())
		_hotspot->GetEditorModel()->ToolMoved(TerrainPosition(_hotspot->GetCapturedTouch()));
}


void EditorGesture::TouchEnded(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
	{
		_hotspot->GetEditorModel()->ToolEnded(TerrainPosition(touch));
		_hotspot->ReleaseTouch(touch);
	}
}


glm::vec2 EditorGesture::TerrainPosition(Touch* touch)
{
	return _hotspot->GetBattleView()->GetTerrainPosition3(touch->GetPosition()).xy();
}
