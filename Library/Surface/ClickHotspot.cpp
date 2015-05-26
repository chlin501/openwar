// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ClickHotspot.h"

#include "Surface.h"
#include "Touch.h"


ClickHotspot::ClickHotspot() :
	_gesture(this),
	_distance(),
	_tolerance(),
	_action(),
	_highlight(false),
	_immediate(false)
{
}


ClickHotspot::~ClickHotspot()
{
}


Gesture* ClickHotspot::GetGesture()
{
	return &_gesture;
}


void ClickHotspot::SetDistance(std::function<glm::vec2(glm::vec2)> distance)
{
	_distance = distance;
}


void ClickHotspot::SetTolerance(std::function<glm::vec2(Touch*)> tolerance)
{
	_tolerance = tolerance;
}


glm::vec2 ClickHotspot::GetDefaultTolerance(Touch* touch, glm::vec2 size)
{
	return glm::max(glm::vec2(24) - size * 0.5f, glm::vec2());
}


float ClickHotspot::GetDistance(glm::vec2 position) const
{
	if (_distance)
		return glm::length(_distance(position));

	return 0;
}


bool ClickHotspot::IsTouchInside(Touch* touch) const
{
	glm::vec2 distance;

	if (_distance)
		distance = _distance(touch->GetCurrentPosition());

	if (_tolerance)
		distance = glm::max(glm::abs(distance) - _tolerance(touch), glm::vec2());

	return glm::length(distance) <= 0;
}


std::function<void ()> ClickHotspot::GetClickAction() const
{
	return _action;
}


void ClickHotspot::SetClickAction(std::function<void()> value)
{
	_action = value;
}


bool ClickHotspot::IsImmediateClick() const
{
	return _immediate;
}


void ClickHotspot::SetImmediateClick(bool value)
{
	_immediate = value;
}


bool ClickHotspot::IsHighlight() const
{
	return _highlight;
}


void ClickHotspot::SetHighlight(bool value)
{
	_highlight = value;
}
