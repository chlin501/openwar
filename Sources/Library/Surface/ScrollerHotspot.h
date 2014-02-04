#ifndef ScrollerHotspot_H
#define ScrollerHotspot_H

#include <glm/glm.hpp>
#include "Hotspot.h"

class Scroller;


class ScrollerHotspot : public Hotspot
{
	Scroller* _scroller;
	glm::vec2 _position;

public:
	ScrollerHotspot(Scroller* scroller, glm::vec2 position);
	virtual ~ScrollerHotspot();

	Scroller* GetScroller() const { return _scroller; }
	glm::vec2 GetPosition() const { return _position; }
};



#endif
