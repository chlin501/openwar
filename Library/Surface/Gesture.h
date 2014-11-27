// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Gesture_H
#define Gesture_H

#include <vector>
#include <glm/glm.hpp>

class Hotspot;
class Surface;
class Touch;


class Gesture
{
public:
	static std::vector<Gesture*>* _gestures;

	Gesture();
	virtual ~Gesture();

	virtual void KeyDown(char key) { }
	virtual void KeyUp(char key) { }

	virtual void ScrollWheel(glm::vec2 position, glm::vec2 delta) { }
	virtual void Magnify(glm::vec2 position, float magnification) { }
	virtual void Magnify(glm::vec2 position) { }

	virtual void TouchWasCreated(Touch* touch) { }
	virtual void TouchWillBeDestroyed(Touch* touch) { }

	virtual void TouchWasCaptured(Touch* touch) { }
	virtual void TouchWillBeReleased(Touch* touch) { }
	virtual void AskReleaseTouchToAnotherHotspot(Touch* touch, Hotspot* anotherHotspot) { }

	virtual void TouchBegin(Touch* touch) { }
	virtual void TouchBegan(Touch* touch) { }
	virtual void TouchMoved(Touch* touch) { }
	virtual void TouchEnded(Touch* touch) { }
};


#endif
