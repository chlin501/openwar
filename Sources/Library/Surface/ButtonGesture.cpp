// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Button.h"
#include "ButtonGesture.h"
#include "ButtonHotspot.h"
#include "Touch.h"
#include "Surface.h"



ButtonGesture::ButtonGesture(Surface* surface) :
_surface(surface),
_buttonControl(nullptr)
{
}



ButtonGesture::~ButtonGesture()
{

}



void ButtonGesture::Update(Surface* surface, double secondsSinceLastUpdate)
{

}



void ButtonGesture::KeyDown(Surface* surface, char key)
{
	/*for (ButtonControl* buttonView : buttonViews)
		if (buttonView->GetWindow() == surface)
			for (ButtonArea* buttonArea : buttonView->GetButtonAreas())
				for (ButtonItem* buttonItem : buttonArea->buttonItems)
					if (buttonItem->HasAction()
							&& !buttonItem->IsDisabled()
							&& buttonItem->GetKeyboardShortcut() == key)
					{
						buttonItem->CallAction();
					}
	*/
}


void ButtonGesture::TouchBegan(Touch* touch)
{
	if (touch->HasGesture() || !_touches.empty())
		return;

	_surface->FindHotspots(glm::mat4(), touch->GetPosition(), [this](Hotspot* hotspot) {
		ButtonHotspot* buttonHotspot = dynamic_cast<ButtonHotspot*>(hotspot);
		if (buttonHotspot != nullptr)
			_buttonControl = buttonHotspot->GetButtonControl();
	});

	if (_buttonControl != nullptr)
	{
		CaptureTouch(touch);
		_buttonControl->SetHighlight(true);
	}

}



void ButtonGesture::TouchMoved()
{
	if (_buttonControl != nullptr && !_touches.empty())
	{
		bool inside = false;

		glm::vec2 p = _touches.front()->GetPosition();
		_surface->FindHotspots(glm::mat4(), p, [this, &inside](Hotspot* hotspot) {
			ButtonHotspot* buttonHotspot = dynamic_cast<ButtonHotspot*>(hotspot);
			if (buttonHotspot != nullptr && buttonHotspot->GetButtonControl() == _buttonControl)
				inside = true;
		});

		_buttonControl->SetHighlight(inside);
	}
}



void ButtonGesture::TouchEnded(Touch* touch)
{
	if (_buttonControl != nullptr)
	{
		std::function<void()> action = _buttonControl->GetAction();
		if (action)
			action();

		_buttonControl->SetHighlight(false);
		_buttonControl = nullptr;
	}
}
