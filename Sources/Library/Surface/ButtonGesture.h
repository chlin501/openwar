// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonGridGesture_H
#define ButtonGridGesture_H

#include "../Surface/Gesture.h"

class ButtonHotspot;
class ButtonItem;
class ButtonGrid;


class ButtonGesture : public Gesture
{
	Surface* _surface;
	ButtonItem* _buttonItem;
	std::shared_ptr<ButtonHotspot> _hotspot;

public:
	std::vector<ButtonGrid*> buttonViews;

	ButtonGesture(Surface* surface);
	virtual ~ButtonGesture();

	virtual void Update(Surface* surface, double secondsSinceLastUpdate);

	virtual void KeyDown(Surface* surface, char key);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);
};


#endif
