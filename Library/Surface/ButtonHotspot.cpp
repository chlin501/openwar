// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonHotspot.h"


ButtonHotspot::ButtonHotspot() :
_action(),
_highlight(false),
_stationary(false),
_immediate(false)
{
}


ButtonHotspot::~ButtonHotspot()
{
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


bool ButtonHotspot::ShouldReleaseWhenMoving() const
{
	return _stationary;
}


void ButtonHotspot::SetReleaseWhenMoving(bool value)
{
	_stationary = value;
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
