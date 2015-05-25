// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ScrollerGesture.h"
#include "ScrollerHotspot.h"
#include "ScrollerViewport.h"
#include "Surface/Touch.h"


ScrollerGesture::ScrollerGesture(ScrollerHotspot* hotspot) :
	_hotspot(hotspot),
	_touch(nullptr)
{
}


ScrollerGesture::~ScrollerGesture()
{
}


void ScrollerGesture::OnRenderLoop(double secondsSinceLastUpdate)
{

}


void ScrollerGesture::TouchWasCaptured(Touch* touch)
{
}


void ScrollerGesture::TouchWillBeReleased(Touch* touch)
{
}


void ScrollerGesture::TouchBegan(Touch* touch)
{
	bounds2f viewportBounds = _hotspot->GetViewport()->GetViewportBounds();
	if (viewportBounds.contains(touch->GetOriginalPosition()))
	{
		_touch = touch;
		_originalContentOffset = _hotspot->GetViewport()->GetContentOffset();
	}
}


void ScrollerGesture::TouchMoved(Touch* touch)
{
	if (touch == _touch)
	{
		glm::vec2 original = _hotspot->GetViewport()->GlobalToLocal(touch->GetOriginalPosition());
		glm::vec2 position = _hotspot->GetViewport()->GlobalToLocal(touch->GetCurrentPosition());
		glm::vec2 offset = _hotspot->GetViewport()->GetClampedOffset(_originalContentOffset + original - position);

		if (!_hotspot->HasCapturedTouch(touch) && touch->HasMoved() && offset != _originalContentOffset)
		{
			_hotspot->TryCaptureTouch(touch);
		}

		if (_hotspot->HasCapturedTouch(touch))
			_hotspot->GetViewport()->SetContentOffset(offset);
	}
}


void ScrollerGesture::TouchEnded(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
		_hotspot->ReleaseTouch(touch);
}
