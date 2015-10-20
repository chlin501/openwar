// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Gesture.h"

#include "Hotspot.h"
#include "Touch.h"
#include <algorithm>


std::vector<Gesture*>* Gesture::_gestures = nullptr;


Gesture::Gesture()
{
	if (_gestures == nullptr)
		_gestures = new std::vector<Gesture*>();
	_gestures->push_back(this);
}


Gesture::~Gesture()
{
	_gestures->erase(
		std::remove(_gestures->begin(), _gestures->end(), this),
		_gestures->end());
}
