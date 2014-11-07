// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <algorithm>
#include "Gesture.h"
#include "Touch.h"
#include "Surface.h"
#include "Hotspot.h"


std::vector<Gesture*>* Gesture::_gestures = nullptr;


Gesture::Gesture(HotspotBase* hotspot) :
	_hotspotBase(hotspot),
	_enabled(true)
{
	if (_gestures == nullptr)
		_gestures = new std::vector<Gesture*>();
	_gestures->push_back(this);
}


Gesture::~Gesture()
{
	for (Touch* touch : _hotspotBase->_touches)
	{
		touch->_gestures.erase(
			std::remove(touch->_gestures.begin(), touch->_gestures.end(), this),
			touch->_gestures.end());
	}

	_gestures->erase(
		std::remove(_gestures->begin(), _gestures->end(), this),
		_gestures->end());
}


HotspotBase* Gesture::GetHotspot() const
{
	return _hotspotBase;
}


void Gesture::RenderHints()
{
}


void Gesture::KeyDown(char key)
{
}


void Gesture::KeyUp(char key)
{
}


void Gesture::ScrollWheel(glm::vec2 position, glm::vec2 delta)
{
}


void Gesture::Magnify(glm::vec2 position, float magnification)
{
}


void Gesture::Magnify(glm::vec2 position)
{
}
