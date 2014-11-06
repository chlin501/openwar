// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonHotspot.h"
#include "Touch.h"
#import "Content.h"


ButtonHotspot::ButtonHotspot(Content* content) :
	_content(content),
	_action(),
	_highlight(false),
	_stationary(false),
	_immediate(false)
{
}


ButtonHotspot::~ButtonHotspot()
{
}


bool ButtonHotspot::IsInside(glm::vec2 position) const
{
	if (_content != nullptr)
	{
		Touch touch(1, position, 0, MouseButtons());
		_content->FindHotspots(&touch);
		for (const std::shared_ptr<HotspotBase> hotspot : touch.GetHotspots())
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
