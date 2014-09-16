#ifndef ScrollerGesture_H
#define ScrollerGesture_H

#include "Gesture.h"

class ScrollerHotspot;


class ScrollerGesture : public Gesture
{
	Surface* _surface;
	std::shared_ptr<ScrollerHotspot> _hotspot;

public:
	ScrollerGesture(Surface* surface);
	virtual ~ScrollerGesture();

	virtual void Update(Surface* surface, double secondsSinceLastUpdate);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);
};



#endif
