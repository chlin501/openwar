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


void ScrollerGesture::TouchCaptured(Touch* touch)
{
}


void ScrollerGesture::TouchReleased(Touch* touch)
{
}


void ScrollerGesture::TouchBegan(Touch* touch)
{
	if (_hotspot->HasCapturedTouch())
		return;

	bounds2f viewportBounds = _hotspot->GetContent()->GetViewportBounds();
	if (viewportBounds.contains(touch->GetOriginal()))
	{
		if (_hotspot->TryCaptureTouch(touch))
		{
			_originalContentOffset = _hotspot->GetContent()->GetContentOffset();
		}
	}
}


void ScrollerGesture::TouchMoved()
{
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
