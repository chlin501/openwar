#include "ScrollerHotspot.h"


ScrollerHotspot::ScrollerHotspot(ScrollerViewport* viewport) :
	_gesture(this),
	_viewport(viewport)
{
}


Gesture* ScrollerHotspot::GetGesture() const
{
	return const_cast<ScrollerGesture*>(&_gesture);
}


ScrollerViewport* ScrollerHotspot::GetViewport() const
{
	return _viewport;
}
