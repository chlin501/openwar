// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ClickGesture_H
#define ClickGesture_H

#include <memory>
#include "Surface/Gesture.h"

class ClickHotspot;
class ButtonItem;
class ButtonGrid;


class ClickGesture : public Gesture
{
	ClickHotspot* _hotspot;

public:
	std::vector<ButtonGrid*> _buttonViews;

	ClickGesture(ClickHotspot* hotspot);
	virtual ~ClickGesture();

	virtual void KeyDown(char key);

	virtual void TouchWasCaptured(Touch* touch);
	virtual void TouchWillBeReleased(Touch* touch);
	virtual void AskReleaseTouchToAnotherHotspot(Touch* touch, Hotspot* anotherHotspot);

	virtual void TouchBegin(Touch* touch);
	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved(Touch* touch);
	virtual void TouchEnded(Touch* touch);

private:
	bool ShouldReleaseTouchToAnotherHotspot(Touch* touch, Hotspot* anotherHotspot);
};


#endif
