// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonGesture.h"
#include "ButtonHotspot.h"
#include "ButtonGrid.h"
#include "Surface.h"
#include "Touch.h"


ButtonGesture::ButtonGesture(ButtonHotspot* hotspot) :
	_hotspot(hotspot)
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


void ButtonGesture::TouchBegan(Touch* touch)
{
	if (touch->IsCaptured() || _hotspot->HasCapturedTouch())
		return;

	if (_hotspot->TryCaptureTouch(touch))
	{
		_hotspot->SetHighlight(true);

		if (_hotspot->IsImmediateClick() && _hotspot->GetClickAction())
			_hotspot->GetClickAction()();
	}
}



void ButtonGesture::TouchMoved(Touch* touch)
{
	if (_hotspot->HasCapturedTouch())
	{
		Touch* touch = _hotspot->GetCapturedTouch();
		bool inside = _hotspot->IsInside(touch->GetCurrentPosition());
		_hotspot->SetHighlight(inside);
	}
}



void ButtonGesture::TouchEnded(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
	{
		bool inside = _hotspot->IsInside(touch->GetCurrentPosition());

		if (inside && !_hotspot->IsImmediateClick() && _hotspot->GetClickAction())
			_hotspot->GetClickAction()();

		_hotspot->ReleaseTouch(touch);
	}
}
