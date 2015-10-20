// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ClickGesture.h"

#include "ClickHotspot.h"
#include "Surface.h"
#include "Touch.h"


ClickGesture::ClickGesture(ClickHotspot* hotspot) :
	_hotspot(hotspot)
{
}


void ClickGesture::TouchWasCaptured(Touch* touch)
{
}


void ClickGesture::TouchWillBeReleased(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
	{
		_hotspot->SetHighlight(false);
	}
}


void ClickGesture::AskReleaseTouchToAnotherHotspot(Touch* touch, Hotspot* anotherHotspot)
{
	if (ShouldReleaseTouchToAnotherHotspot(touch, anotherHotspot))
		_hotspot->ReleaseTouch(touch);
}


void ClickGesture::TouchBegin(Touch* touch)
{
	if (!_hotspot->HasCapturedTouch())
		_hotspot->TryCaptureTouch(touch);
}


void ClickGesture::TouchBegan(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
	{
		if (_hotspot->IsImmediateClick())
		{
			if (_hotspot->GetClickAction())
				_hotspot->GetClickAction()();
		}
		else
		{
			_hotspot->SetHighlight(true);
		}
	}
}


void ClickGesture::TouchMoved(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
	{
		if (!_hotspot->IsImmediateClick())
		{
			bool inside = _hotspot->IsTouchInside(touch);
			_hotspot->SetHighlight(inside);
		}
	}
}


void ClickGesture::TouchEnded(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
	{
		if (!_hotspot->IsImmediateClick())
		{
			bool inside = _hotspot->IsTouchInside(touch);

			if (inside && !_hotspot->IsImmediateClick() && _hotspot->GetClickAction())
				_hotspot->GetClickAction()();
		}
		_hotspot->ReleaseTouch(touch);
	}
}


bool ClickGesture::ShouldReleaseTouchToAnotherHotspot(Touch* touch, Hotspot* anotherHotspot)
{
	ClickHotspot* clickHotspot = dynamic_cast<ClickHotspot*>(anotherHotspot);
	if (clickHotspot == nullptr)
		return true;

	if (_hotspot->IsImmediateClick() && !clickHotspot->IsImmediateClick())
		return true;

	glm::vec2 position = touch->GetCurrentPosition();
	return clickHotspot->GetDistance(position) < _hotspot->GetDistance(position);
}
