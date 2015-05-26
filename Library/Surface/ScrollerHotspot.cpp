// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ScrollerHotspot.h"


ScrollerHotspot::ScrollerHotspot(ScrollableViewport2D* viewport) :
	_gesture(this),
	_viewport(viewport)
{
}


Gesture* ScrollerHotspot::GetGesture()
{
	return &_gesture;
}


ScrollableViewport2D* ScrollerHotspot::GetViewport() const
{
	return _viewport;
}
