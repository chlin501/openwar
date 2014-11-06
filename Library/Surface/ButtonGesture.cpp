// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonGesture.h"
#include "ButtonHotspot.h"
#include "ButtonGrid.h"
#include "Surface.h"
#include "Touch.h"


ButtonGesture::ButtonGesture(ButtonHotspot* hotspot) : Gesture(hotspot),
	_hotspot(hotspot),
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
	{
		CaptureTouch(touch);

		_hotspot->SetHighlight(true);

		if (_hotspot->IsImmediateClick() && _hotspot->GetClickAction())
			_hotspot->GetClickAction()();
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
		bool inside = _hotspot->IsInside(touch->GetPosition());
		_hotspot->SetHighlight(inside);
	}
}



void ButtonGesture::TouchEnded(Touch* touch)
{
	if (_hotspot != nullptr)
	{
		bool inside = _hotspot->IsInside(touch->GetPosition());

		if (inside && !_hotspot->IsImmediateClick() && _hotspot->GetClickAction())
			_hotspot->GetClickAction()();

		_hotspot->SetHighlight(false);
		ReleaseTouch(touch);
	}
}
