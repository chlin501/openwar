#include "Scroller.h"
#include "ScrollerGesture.h"
#include "ScrollerHotspot.h"
#include "Touch.h"
#include "Surface.h"


ScrollerGesture::ScrollerGesture(ScrollerHotspot* scrollerHotspot) : Gesture(scrollerHotspot),
_hotspot(nullptr)
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
	if (/*touch->HasGesture() ||*/ !_touches.empty())
		return;

	if (_hotspot != nullptr)
		return;

	std::shared_ptr<ScrollerHotspot> scrollerHotspot;

	for (std::shared_ptr<HotspotBase> hotspot : touch->GetHotspots())
		if (scrollerHotspot == nullptr)
			scrollerHotspot = std::dynamic_pointer_cast<ScrollerHotspot>(hotspot);

	if (scrollerHotspot != nullptr)
	{
		CaptureTouch(touch);
		_hotspot = scrollerHotspot;
	}
}


void ScrollerGesture::TouchMoved()
{
	if (_hotspot != nullptr && !_touches.empty())
	{
		glm::vec2 position = _touches.front()->GetPosition();

		_hotspot->ScrollToPosition(position);
	}
}


void ScrollerGesture::TouchEnded(Touch* touch)
{
	_hotspot = nullptr;
}
