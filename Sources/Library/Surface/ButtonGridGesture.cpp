// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonGridGesture.h"
#include "ButtonGrid.h"
#include "../Surface/Touch.h"



ButtonGridGesture::ButtonGridGesture() :
_buttonItem(nullptr)
{
}



ButtonGridGesture::~ButtonGridGesture()
{

}



void ButtonGridGesture::Update(Surface* surface, double secondsSinceLastUpdate)
{

}



void ButtonGridGesture::KeyDown(Surface* surface, char key)
{
	for (ButtonGrid* buttonView : buttonViews)
		if (buttonView->GetSurface() == surface)
			for (ButtonArea* buttonArea : buttonView->GetButtonAreas())
				for (ButtonItem* buttonItem : buttonArea->buttonItems)
					if (buttonItem->HasAction()
							&& !buttonItem->IsDisabled()
							&& buttonItem->GetKeyboardShortcut() == key)
					{
						buttonItem->CallAction();
					}

}


void ButtonGridGesture::TouchBegan(Touch* touch)
{
	if (touch->HasGesture() || !_touches.empty())
		return;

	for (ButtonGrid* buttonView : buttonViews)
		if (buttonView->GetSurface() == touch->GetSurface())
			for (ButtonArea* buttonArea : buttonView->GetButtonAreas())
				for (ButtonItem* buttonItem : buttonArea->buttonItems)
					if (buttonItem->HasAction()
							&& !buttonItem->IsDisabled()
							&& buttonItem->GetBounds().contains(touch->GetPosition()))
					{
						_buttonItem = buttonItem;
					}

	if (_buttonItem != nullptr)
	{
		CaptureTouch(touch);
		_buttonItem->SetHighlight(true);
	}
	else
	{
		for (ButtonGrid* buttonView : buttonViews)
			if (buttonView->GetSurface() == touch->GetSurface())
				for (ButtonArea* buttonArea : buttonView->GetButtonAreas())
				{
					buttonArea->noaction();
				}
	}
}



void ButtonGridGesture::TouchMoved()
{
	if (_buttonItem != nullptr && !_touches.empty())
	{
		_buttonItem->SetHighlight(_buttonItem->GetBounds().contains(_touches.front()->GetPosition()));
	}
}



void ButtonGridGesture::TouchEnded(Touch* touch)
{
	if (_buttonItem != nullptr)
	{
		if (_buttonItem->GetBounds().contains(touch->GetPosition()))
			_buttonItem->CallAction();

		_buttonItem->SetHighlight(false);
		_buttonItem = nullptr;
	}
}
