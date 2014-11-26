// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonGesture.h"
#include "ButtonHotspot.h"
#include "ButtonGrid.h"
#include "Surface/Surface.h"
#include "Surface/Touch.h"


ButtonGesture::ButtonGesture(ButtonHotspot* hotspot) :
	_hotspot(hotspot)
{
}


ButtonGesture::~ButtonGesture()
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


void ButtonGesture::TouchWasCaptured(Touch* touch)
{
}


void ButtonGesture::TouchWillBeReleased(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
	{
		_hotspot->SetHighlight(false);
	}
}


void ButtonGesture::AskReleaseTouchToAnotherHotspot(Touch* touch, Hotspot* anotherHotspot)
{
	bool release = false;
	ButtonHotspot* buttonHotspot = dynamic_cast<ButtonHotspot*>(anotherHotspot);
	if (buttonHotspot != nullptr)
	{
		glm::vec2 position = touch->GetCurrentPosition();
		release = buttonHotspot->GetDistance(position) < _hotspot->GetDistance(position);
	}
	else
	{
		release = true;
	}

	if (release)
		_hotspot->ReleaseTouch(touch);
}


void ButtonGesture::TouchBegan(Touch* touch)
{
	if (_hotspot->HasCapturedTouch())
		return;

	if (_hotspot->TryCaptureTouch(touch))
	{
		_hotspot->SetHighlight(true);

		if (_hotspot->IsImmediateClick() && _hotspot->GetClickAction())
		{
			_hotspot->GetClickAction()();
			_hotspot->ReleaseTouch(touch);
		}
	}
}



void ButtonGesture::TouchMoved(Touch* touch)
{
	if (_hotspot->HasCapturedTouch())
	{
		Touch* touch = _hotspot->GetCapturedTouch();
		bool inside = _hotspot->IsTouchInside(touch);
		_hotspot->SetHighlight(inside);
	}
}



void ButtonGesture::TouchEnded(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
	{
		bool inside = _hotspot->IsTouchInside(touch);

		if (inside && !_hotspot->IsImmediateClick() && _hotspot->GetClickAction())
			_hotspot->GetClickAction()();

		_hotspot->ReleaseTouch(touch);
	}
}
