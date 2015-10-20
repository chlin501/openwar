// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ScrollerHotspot_H
#define ScrollerHotspot_H

#include "Hotspot.h"
#include "ScrollerGesture.h"
#include "Algebra/bounds.h"
#include <functional>
#include <glm/glm.hpp>

class ScrollerViewport;


class ScrollerHotspot : public Hotspot
{
	ScrollerGesture _gesture;
	ScrollerViewport& _viewport;

public:
	ScrollerHotspot(ScrollerViewport& viewport);

	const ScrollerViewport& GetScrollerViewport() const { return _viewport; }
	ScrollerViewport& GetScrollerViewport() { return _viewport; }

public: // Hotspot
	Gesture* GetGesture() override;
};


#endif
