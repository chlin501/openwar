// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonGesture.h"
#include "ButtonHotspot.h"
#include "ButtonGrid.h"
#include "Surface.h"
#include "Touch.h"

ButtonGesture::ButtonGesture() :
_buttonItem(nullptr)
{
}



ButtonGesture::~ButtonGesture()
{

}



void ButtonGesture::Update(double secondsSinceLastUpdate)
{

}



void ButtonGesture::KeyDown(char key)
{
	for (ButtonGrid* buttonView : _buttonViews)
		for (ButtonArea* buttonArea : buttonView->GetButtonAreas())
			for (ButtonItem* buttonItem : buttonArea->buttonItems)
				if (buttonItem->HasAction()
						&& !buttonItem->IsDisabled()
						&& buttonItem->GetKeyboardShortcut() == key)
				{
					buttonItem->CallAction();
				}
}


void ButtonGesture::TouchBegan(Touch* touch)
{
	if (_hotspot != nullptr)
		return;
	if (/*touch->HasGesture() ||*/ !_touches.empty())
		return;

	for (std::shared_ptr<Hotspot> hotspot : touch->GetHotspots())
		if (_hotspot == nullptr)
			_hotspot = std::dynamic_pointer_cast<ButtonHotspot>(hotspot);

	if (_hotspot != nullptr)
	{
		CaptureTouch(touch);

		_hotspot->SetHighlight(true);

		if (_hotspot->IsImmediate() && _hotspot->GetAction())
			_hotspot->GetAction()();
	}
	else
	{
		for (ButtonGrid* buttonView : _buttonViews)
			for (ButtonArea* buttonArea : buttonView->GetButtonAreas())
				buttonArea->noaction();
	}
}



void ButtonGesture::TouchMoved()
{
	if (_hotspot != nullptr)
	{
		Touch* touch = _touches.front();
		bool found = false;

		if (_hotspot->IsStationary() && touch->GetMotion() == Motion::Moving)
		{
			UncaptureTouch(touch);
			_hotspot->SetHighlight(false);
			_hotspot = nullptr;
		}
		else
		{
			for (std::shared_ptr<Hotspot> hotspot : touch->GetHotspots())
				if (hotspot == _hotspot)
					found = true;

			_hotspot->SetHighlight(found);
		}
	}
}



void ButtonGesture::TouchEnded(Touch* touch)
{
	if (_hotspot != nullptr)
	{
		bool found = false;

		for (std::shared_ptr<Hotspot> hotspot : touch->GetHotspots())
			if (hotspot == _hotspot)
				found = true;

		if (found && !_hotspot->IsImmediate() && _hotspot->GetAction())
			_hotspot->GetAction()();

		_hotspot->SetHighlight(false);
		_hotspot = nullptr;
	}
}
