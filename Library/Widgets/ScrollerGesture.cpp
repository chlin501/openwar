#include "ScrollerGesture.h"
#include "ScrollerHotspot.h"
#include "Touch.h"
#include "Content.h"


ScrollerGesture::ScrollerGesture(ScrollerHotspot* hotspot) :
	_hotspot(hotspot),
	_touch(nullptr)
{
}


ScrollerGesture::~ScrollerGesture()
{
}


void ScrollerGesture::Update(double secondsSinceLastUpdate)
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
	bounds2f viewportBounds = _hotspot->GetContent()->GetViewportBounds();
	if (viewportBounds.contains(touch->GetOriginalPosition()))
	{
		_touch = touch;
		_originalContentOffset = _hotspot->GetContent()->GetContentOffset();
	}
}


void ScrollerGesture::TouchMoved(Touch* touch)
{
	if (touch == _touch)
	{
		glm::vec2 original = _hotspot->GetContent()->GlobalToLocal(touch->GetOriginalPosition());
		glm::vec2 position = _hotspot->GetContent()->GlobalToLocal(touch->GetCurrentPosition());
		glm::vec2 offset = _hotspot->GetContent()->ClampContentOffset(_originalContentOffset + original - position);

		if (!_hotspot->HasCapturedTouch(touch) && touch->HasMoved() && offset != _originalContentOffset)
		{
			_hotspot->TryCaptureTouch(touch);
		}

		if (_hotspot->HasCapturedTouch(touch))
			_hotspot->GetContent()->SetContentOffset(offset);
	}
}


void ScrollerGesture::TouchEnded(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
		_hotspot->ReleaseTouch(touch);
}
