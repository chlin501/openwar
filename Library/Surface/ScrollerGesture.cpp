#include "Scroller.h"
#include "ScrollerGesture.h"
#include "ScrollerHotspot.h"
#include "Touch.h"
#include "Surface.h"


ScrollerGesture::ScrollerGesture(ScrollerHotspot* hotspot) : Gesture(hotspot),
_hotspot(hotspot)
{
}


ScrollerGesture::~ScrollerGesture()
{
}


void ScrollerGesture::Update(double secondsSinceLastUpdate)
{

}


void ScrollerGesture::TouchBegan(Touch* touch)
{
	bounds2f viewportBounds = _hotspot->GetContent()->GetViewportBounds();
	if (viewportBounds.contains(touch->GetOriginal()))
	{
		CaptureTouch(touch);
		_originalContentOffset = _hotspot->GetContent()->GetContentOffset();
	}
}


void ScrollerGesture::TouchMoved()
{
	if (!_touches.empty())
	{
		glm::vec2 original = _hotspot->GetContent()->ViewportToContent(_touches.front()->GetOriginal());
		glm::vec2 position = _hotspot->GetContent()->ViewportToContent(_touches.front()->GetPosition());
		_hotspot->GetContent()->SetContentOffset(_originalContentOffset + original - position);
	}
}


void ScrollerGesture::TouchEnded(Touch* touch)
{
	ReleaseTouch(touch);
}
