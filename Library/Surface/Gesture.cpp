// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <algorithm>
#include "Gesture.h"
#include "Touch.h"
#include "Surface.h"


std::vector<Gesture*>* Gesture::_gestures = nullptr;


Gesture::Gesture(HotspotBase* hotspot) :
	_hotspot(hotspot),
	_enabled(true)
{
	if (_gestures == nullptr)
		_gestures = new std::vector<Gesture*>();
	_gestures->push_back(this);
}


Gesture::~Gesture()
{
	for (Touch* touch : _touches)
	{
		touch->_gestures.erase(
			std::remove(touch->_gestures.begin(), touch->_gestures.end(), this),
			touch->_gestures.end());
	}

	_gestures->erase(
		std::remove(_gestures->begin(), _gestures->end(), this),
		_gestures->end());
}


/*void Gesture::Update(double secondsSinceLastUpdate)
{
}*/



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


void Gesture::TouchWasCancelled(Touch* touch)
{
}


void Gesture::CaptureTouch(Touch* touch)
{
	_touches.push_back(touch);
	touch->_gestures.push_back(this);
}


void Gesture::UncaptureTouch(Touch* touch)
{
	_touches.erase(
		std::remove(_touches.begin(), _touches.end(), touch),
		_touches.end());

	touch->_gestures.erase(
		std::remove(touch->_gestures.begin(), touch->_gestures.end(), this),
		touch->_gestures.end());
}
