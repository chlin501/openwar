#include "ScrollerHotspot.h"


ScrollerHotspot::ScrollerHotspot(WidgetViewport* viewport) :
	_gesture(this),
	_viewport(viewport)
{
}


Gesture* ScrollerHotspot::GetGesture() const
{
	return const_cast<ScrollerGesture*>(&_gesture);
}


WidgetViewport* ScrollerHotspot::GetViewport() const
{
	return _viewport;
}
