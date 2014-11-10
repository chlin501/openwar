// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonHotspot.h"
#include "Touch.h"
#include "Surface.h"


ButtonHotspot::ButtonHotspot(std::function<void(Touch*)> findHotspots) :
	_gesture(this),
	_findHotspots(findHotspots),
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
	if (_findHotspots)
	{
		Touch touch(1, position, 0, MouseButtons());
		_findHotspots(&touch);
		for (std::shared_ptr<Hotspot> hotspot : touch._subscribedHotspots)
			if (hotspot.get() == this)
				return true;
	}

	return false;
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
