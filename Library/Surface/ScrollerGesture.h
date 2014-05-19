#ifndef ScrollerGesture_H
#define ScrollerGesture_H

#include "Gesture.h"

class Scroller;


class ScrollerGesture : public Gesture
{
	Surface* _surface;
	Scroller* _scroller;
	glm::vec2 _position;

public:
	ScrollerGesture(Surface* surface);
	virtual ~ScrollerGesture();

	virtual void Update(Surface* surface, double secondsSinceLastUpdate);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);
};



#endif
