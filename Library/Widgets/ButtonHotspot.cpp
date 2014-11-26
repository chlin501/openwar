// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonHotspot.h"
#include "Surface/Touch.h"
#include "Surface/Surface.h"
#import "geometry.h"


ButtonHotspot::ButtonHotspot() :
	_gesture(this),
	_distance(),
	_tolerance(),
	_action(),
	_highlight(false),
	_immediate(false)
{
}


ButtonHotspot::~ButtonHotspot()
{
}


Gesture* ButtonHotspot::GetGesture()
{
	return &_gesture;
}


void ButtonHotspot::SetDistance(std::function<glm::vec2(glm::vec2)> distance)
{
	_distance = distance;
}


void ButtonHotspot::SetTolerance(std::function<glm::vec2(Touch*)> tolerance)
{
	_tolerance = tolerance;
}


glm::vec2 ButtonHotspot::GetDefaultTolerance(Touch* touch, glm::vec2 size)
{
	return glm::max(glm::vec2(24) - size * 0.5f, glm::vec2());
}


float ButtonHotspot::GetDistance(glm::vec2 position) const
{
	if (_distance)
		return glm::length(_distance(position));

	return 0;
}


bool ButtonHotspot::IsTouchInside(Touch* touch) const
{
	glm::vec2 distance;

	if (_distance)
		distance = _distance(touch->GetCurrentPosition());

	if (_tolerance)
		distance = glm::max(glm::abs(distance) - _tolerance(touch), glm::vec2());

	return glm::length(distance) <= 0;
}


std::function<void ()> ButtonHotspot::GetClickAction() const
{
	return _action;
}


void ButtonHotspot::SetClickAction(std::function<void()> value)
{
	_action = value;
}


bool ButtonHotspot::IsImmediateClick() const
{
	return _immediate;
}


void ButtonHotspot::SetImmediateClick(bool value)
{
	_immediate = value;
}


bool ButtonHotspot::IsHighlight() const
{
	return _highlight;
}


void ButtonHotspot::SetHighlight(bool value)
{
	_highlight = value;
}
