// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonGridGesture_H
#define ButtonGridGesture_H

#include "../Surface/Gesture.h"

class ButtonItem;
class ButtonGrid;


class ButtonGridGesture : public Gesture
{
	ButtonItem* _buttonItem;

public:
	std::vector<ButtonGrid*> buttonViews;

	ButtonGridGesture();
	virtual ~ButtonGridGesture();

	virtual void Update(Surface* surface, double secondsSinceLastUpdate);

	virtual void KeyDown(Surface* surface, char key);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);
};


#endif
