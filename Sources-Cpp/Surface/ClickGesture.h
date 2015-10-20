// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ClickGesture_H
#define ClickGesture_H

#include "Gesture.h"
#include <memory>

class ClickHotspot;


class ClickGesture : public Gesture
{
	ClickHotspot* _hotspot;

public:
	ClickGesture(ClickHotspot* hotspot);

public: // Gesture
	void TouchWasCaptured(Touch* touch) override;
	void TouchWillBeReleased(Touch* touch) override;
	void AskReleaseTouchToAnotherHotspot(Touch* touch, Hotspot* anotherHotspot) override;

	void TouchBegin(Touch* touch) override;
	void TouchBegan(Touch* touch) override;
	void TouchMoved(Touch* touch) override;
	void TouchEnded(Touch* touch) override;

private:
	bool ShouldReleaseTouchToAnotherHotspot(Touch* touch, Hotspot* anotherHotspot);
};


#endif
