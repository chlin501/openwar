// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef EDITORGESTURE_H
#define EDITORGESTURE_H

#include "Gestures/Gesture.h"

class BattleView;
class EditorHotspot;


class EditorGesture : public Gesture
{
	EditorHotspot* _hotspot;

public:
	EditorGesture(EditorHotspot* hotspot);

	virtual void Update(double secondsSinceLastUpdate);

	virtual void TouchWasCaptured(Touch* touch);
	virtual void TouchWillBeReleased(Touch* touch);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved(Touch* touch);
	virtual void TouchEnded(Touch* touch);

private:
	glm::vec2 TerrainPosition(Touch* touch);
};


#endif
