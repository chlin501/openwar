#ifndef ScrollerHotspot_H
#define ScrollerHotspot_H

#include <functional>
#include <glm/glm.hpp>
#include "Hotspot.h"
#include "ScrollerGesture.h"
#include "bounds.h"

class ScrollerViewport;


class ScrollerHotspot : public Hotspot
{
	ScrollerGesture _gesture;
	ScrollerViewport* _viewport;

public:
	ScrollerHotspot(ScrollerViewport* viewport);

	virtual Gesture* GetGesture() const;

	ScrollerViewport* GetViewport() const;
};


#endif
