// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ScrollerGesture.h"

#include "ScrollerHotspot.h"
#include "Touch.h"
#include "Graphics/Viewport.h"


ScrollerGesture::ScrollerGesture(ScrollerHotspot* hotspot) :
	_hotspot(hotspot),
	_touch(nullptr)
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
	bounds2f viewportBounds = _hotspot->GetScrollerViewport().GetViewportBounds();
	if (viewportBounds.contains(touch->GetOriginalPosition()))
	{
		_touch = touch;
		_originalContentOffset = _hotspot->GetScrollerViewport().GetContentOffset();
	}
}


void ScrollerGesture::TouchMoved(Touch* touch)
{
	if (touch == _touch)
	{
		glm::vec2 original = _hotspot->GetScrollerViewport().ScreenToContent(touch->GetOriginalPosition());
		glm::vec2 position = _hotspot->GetScrollerViewport().ScreenToContent(touch->GetCurrentPosition());
		glm::vec2 offset = _hotspot->GetScrollerViewport().GetClampedOffset(_originalContentOffset + original - position);

		if (!_hotspot->HasCapturedTouch(touch) && touch->HasMoved() && offset != _originalContentOffset)
		{
			_hotspot->TryCaptureTouch(touch);
		}

		if (_hotspot->HasCapturedTouch(touch))
			_hotspot->GetScrollerViewport().SetContentOffset(offset);
	}
}


void ScrollerGesture::TouchEnded(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
		_hotspot->ReleaseTouch(touch);
}
