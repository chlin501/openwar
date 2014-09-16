#ifndef ScrollerHotspot_H
#define ScrollerHotspot_H

#include <glm/glm.hpp>
#include "Hotspot.h"

class Scroller;


class ScrollerHotspot : public Hotspot
{
	glm::vec2 _position;
	Scroller* _scroller;

public:
	ScrollerHotspot(glm::vec2 position, Scroller* scroller);
	virtual ~ScrollerHotspot();

	glm::vec2 GetPosition() const { return _position; }

	void ScrollToPosition(glm::vec2 position);
};



#endif
