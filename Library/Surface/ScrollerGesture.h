#ifndef ScrollerGesture_H
#define ScrollerGesture_H

#include "Gesture.h"

class ScrollerHotspot;


class ScrollerGesture : public Gesture
{
	ScrollerHotspot* _hotspot;

public:
	ScrollerGesture(ScrollerHotspot* hotspot);
	virtual ~ScrollerGesture();

	virtual void Update(double secondsSinceLastUpdate);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);
};



#endif
