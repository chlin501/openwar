#include "Scroller.h"
#include "ScrollerGesture.h"
#include "ScrollerHotspot.h"
#include "Touch.h"


ScrollerGesture::ScrollerGesture(ScrollerHotspot* hotspot) :
_hotspot(hotspot)
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
}


void ScrollerGesture::TouchMoved(Touch* touch)
{
	if (!_hotspot->HasCapturedTouch())
	{
		for (Touch* touch : _hotspot->GetSubscribedTouches())
			if (touch->HasMoved() && _hotspot->TryCaptureTouch(touch))
			{
				_originalContentOffset = _hotspot->GetContent()->GetContentOffset();
			}
	}

	if (_hotspot->HasCapturedTouch())
	{
		glm::vec2 original = _hotspot->GetContent()->ViewportToContent(_hotspot->GetCapturedTouch()->GetOriginal());
		glm::vec2 position = _hotspot->GetContent()->ViewportToContent(_hotspot->GetCapturedTouch()->GetPosition());
		_hotspot->GetContent()->SetContentOffset(_originalContentOffset + original - position);
	}
}


void ScrollerGesture::TouchEnded(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
		_hotspot->ReleaseTouch(touch);
}
