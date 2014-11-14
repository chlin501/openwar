// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonHotspot.h"
#include "Touch.h"
#include "Surface.h"


ButtonHotspot::ButtonHotspot(std::function<bool(glm::vec2)> inside) :
	_gesture(this),
	_inside(inside),
	_action(),
	_highlight(false),
	_immediate(false)
{
}


ButtonHotspot::~ButtonHotspot()
{
}


Gesture* ButtonHotspot::GetGesture() const
{
	return const_cast<ButtonGesture*>(&_gesture);
}


bool ButtonHotspot::IsInside(glm::vec2 position) const
{
	return _inside(position);
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
