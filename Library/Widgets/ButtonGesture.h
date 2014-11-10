// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonGridGesture_H
#define ButtonGridGesture_H

#include <memory>
#include "Surface/Gesture.h"

class ButtonHotspot;
class ButtonItem;
class ButtonGrid;


class ButtonGesture : public Gesture
{
	ButtonHotspot* _hotspot;

public:
	std::vector<ButtonGrid*> _buttonViews;

	ButtonGesture(ButtonHotspot* hotspot);
	virtual ~ButtonGesture();

	virtual void KeyDown(char key);

	virtual void TouchWasCaptured(Touch* touch);
	virtual void TouchWillBeReleased(Touch* touch);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved(Touch* touch);
	virtual void TouchEnded(Touch* touch);
};


#endif