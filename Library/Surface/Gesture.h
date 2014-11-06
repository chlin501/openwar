// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef GESTURE_H
#define GESTURE_H

#include <vector>
#include <glm/glm.hpp>

class HotspotBase;
class Surface;
class Touch;


class Gesture
{
	HotspotBase* _hotspot;
	bool _enabled;

public:
	static std::vector<Gesture*>* _gestures;
	std::vector<Touch*> _touches;

	Gesture(HotspotBase* hotspot);
	virtual ~Gesture();

	bool IsEnabled() const { return _enabled; }
	void SetEnabled(bool value) { _enabled = value; }

	virtual void Update(double secondsSinceLastUpdate) = 0;
	virtual void RenderHints();

	virtual void KeyDown(char key);
	virtual void KeyUp(char key);

	virtual void ScrollWheel(glm::vec2 position, glm::vec2 delta);
	virtual void Magnify(glm::vec2 position, float magnification);
	virtual void Magnify(glm::vec2 position);

	virtual void TouchBegan(Touch* touch) = 0;
	virtual void TouchMoved() = 0;
	virtual void TouchEnded(Touch* touch) = 0;

	virtual void TouchWasCancelled(Touch* touch);

	void CaptureTouch(Touch* touch);
	void UncaptureTouch(Touch* touch);
};


#endif
