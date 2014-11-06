// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "EditorGesture.h"
#include "BattleView/BattleView.h"
#include "Surface/Touch.h"
#include "EditorModel.h"
#include "EditorHotspot.h"



EditorGesture::EditorGesture(EditorHotspot* hotspot) : Gesture(hotspot),
	_battleView(hotspot->GetBattleView()),
	_editorModel(hotspot->GetEditorModel())
{
}


void EditorGesture::Update(double secondsSinceLastUpdate)
{

}


void EditorGesture::TouchBegan(Touch* touch)
{
	if (_editorModel->GetEditorMode() == EditorMode::Hand)
	{
		bounds2f b = _battleView->GetTerrainBounds();
		glm::vec2 p = (TerrainPosition(touch) - b.min) / b.size();

		static glm::vec2 old;
		if (old != glm::vec2())
		{
			glm::vec2 d = p - old;
			float a = 90 - glm::round(glm::degrees(glm::atan(d.y, d.x)));
			while (a > 360)
				a -= 360;
			while (a < 0)
				a += 360;
			//NSLog(@"Angle: %g", a);
		}
		old = p;

		//NSLog(@"Position: %g, %g", p.x, p.y);
		return;
	}

	if (touch->HasGesture() || !_touches.empty())
		return;

	_editorModel->ToolBegan(TerrainPosition(touch));
	CaptureTouch(touch);
}


void EditorGesture::TouchMoved()
{
	if (!_touches.empty())
		_editorModel->ToolMoved(TerrainPosition(_touches.front()));
}


void EditorGesture::TouchEnded(Touch* touch)
{
	_editorModel->ToolEnded(TerrainPosition(touch));

}


void EditorGesture::TouchWasCancelled(Touch* touch)
{

}


glm::vec2 EditorGesture::TerrainPosition(Touch* touch)
{
	return _battleView->GetTerrainPosition3(touch->GetPosition()).xy();
}
