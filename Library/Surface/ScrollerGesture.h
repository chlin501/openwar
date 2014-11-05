#ifndef ScrollerGesture_H
#define ScrollerGesture_H

#include "Gesture.h"

class ScrollerHotspot;


class ScrollerGesture : public Gesture
{
	std::shared_ptr<ScrollerHotspot> _hotspot;

public:
	ScrollerGesture();
	virtual ~ScrollerGesture();

	virtual void Update(double secondsSinceLastUpdate);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);
};



#endif
