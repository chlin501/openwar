#ifndef ScrollerHotspot_H
#define ScrollerHotspot_H

#include <glm/glm.hpp>
#include "Hotspot.h"

class Scroller;


class ScrollerHotspot : public Hotspot
{
	glm::vec2 _position;
	Scroller* _scroller;
	std::function<void(glm::vec2)> _action;

public:
	ScrollerHotspot(glm::vec2 position, Scroller* scroller);
	virtual ~ScrollerHotspot();

	void OnScrollToPosition(std::function<void(glm::vec2)> action);

	glm::vec2 GetPosition() const;

	void ScrollToPosition(glm::vec2 position);
};



#endif
