// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ScrollerGesture_H
#define ScrollerGesture_H

#include "Gesture.h"
#include "RenderLoopObserver.h"

class ScrollerHotspot;


class ScrollerGesture : public Gesture, RenderLoopObserver
{
	ScrollerHotspot* _hotspot;
	Touch* _touch;
	glm::vec2 _originalContentOffset;

public:
	ScrollerGesture(ScrollerHotspot* hotspot);
	virtual ~ScrollerGesture();

private: // RenderLoopObserver
	virtual void OnRenderLoop(double secondsSinceLastUpdate);

public:
	virtual void TouchWasCaptured(Touch* touch);
	virtual void TouchWillBeReleased(Touch* touch);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved(Touch* touch);
	virtual void TouchEnded(Touch* touch);
};



#endif
