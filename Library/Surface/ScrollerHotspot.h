#ifndef ScrollerHotspot_H
#define ScrollerHotspot_H

#include <functional>
#include <glm/glm.hpp>
#include "Hotspot.h"
#include "ScrollerGesture.h"
#include "bounds.h"

class Content;


class ScrollerHotspot : public Hotspot<ScrollerHotspot, ScrollerGesture>
{
	Content* _content;
public:
	ScrollerHotspot(Content* content);

	Content* GetContent() const;
};


#endif
