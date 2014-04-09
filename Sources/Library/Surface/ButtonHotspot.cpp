// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonHotspot.h"


ButtonHotspot::ButtonHotspot() :
_action(),
_stationary(false),
_highlight(false),
_immediate(false)
{
}


ButtonHotspot::~ButtonHotspot()
{
}


std::function<void ()> ButtonHotspot::GetAction() const
{
	return _action;
}


void ButtonHotspot::SetAction(std::function<void ()> value)
{
	_action = value;
}


bool ButtonHotspot::IsImmediate() const
{
	return _immediate;
}


bool ButtonHotspot::IsStationary() const
{
	return _stationary;
}


void ButtonHotspot::SetStationary(bool value)
{
	_stationary = value;
}


void ButtonHotspot::SetImmediate(bool value)
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
