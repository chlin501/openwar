#include "ScrollerHotspot.h"
#import "Content.h"


ScrollerHotspot::ScrollerHotspot(Content* content) :
	_gesture(this),
	_content(content)
{
}


Gesture* ScrollerHotspot::GetGesture() const
{
	return const_cast<ScrollerGesture*>(&_gesture);
}


Content* ScrollerHotspot::GetContent() const
{
	return _content;
}
