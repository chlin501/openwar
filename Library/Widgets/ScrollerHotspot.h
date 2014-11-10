#ifndef ScrollerHotspot_H
#define ScrollerHotspot_H

#include <functional>
#include <glm/glm.hpp>
#include "Hotspot.h"
#include "ScrollerGesture.h"
#include "bounds.h"

class WidgetViewport;


class ScrollerHotspot : public Hotspot
{
	ScrollerGesture _gesture;
	WidgetViewport* _viewport;

public:
	ScrollerHotspot(WidgetViewport* viewport);

	virtual Gesture* GetGesture() const;

	WidgetViewport* GetViewport() const;
};


#endif
