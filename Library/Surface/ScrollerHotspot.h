#ifndef ScrollerHotspot_H
#define ScrollerHotspot_H

#include <functional>
#include <glm/glm.hpp>
#include "Hotspot.h"
#include "ScrollerGesture.h"
#include "bounds.h"

class Content;


class ScrollerHotspot : public Hotspot
{
	ScrollerGesture _gesture;
	Content* _content;

public:
	ScrollerHotspot(Content* content);

	virtual Gesture* GetGesture() const;

	Content* GetContent() const;
};


#endif
