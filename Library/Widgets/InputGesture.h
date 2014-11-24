#ifndef InputGesture_H
#define InputGesture_H

#include "Surface/Gesture.h"

class InputHotspot;


class InputGesture : public Gesture
{
	InputHotspot* _inputHotspot;

public:
	InputGesture(InputHotspot* inputHotspot);
	virtual ~InputGesture();

	virtual void TouchBegan(Touch* touch);
	virtual void TouchEnded(Touch* touch);
};


#endif
