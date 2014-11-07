// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Hotspot.h"
#include "Touch.h"


HotspotBase::HotspotBase()
{
}


HotspotBase::~HotspotBase()
{
}


void HotspotBase::CaptureTouch(Touch* touch)
{
	_touches.push_back(touch);
	touch->_gestures.push_back(GetGesture());
}


void HotspotBase::ReleaseTouch(Touch* touch)
{
	_touches.erase(
		std::remove(_touches.begin(), _touches.end(), touch),
		_touches.end());

	touch->_gestures.erase(
		std::remove(touch->_gestures.begin(), touch->_gestures.end(), GetGesture()),
		touch->_gestures.end());
}
