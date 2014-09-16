#include "ScrollerHotspot.h"
#include "Scroller.h"


ScrollerHotspot::ScrollerHotspot(glm::vec2 position, Scroller* scroller) :
	_position(position),
	_scroller(scroller)
{
}


ScrollerHotspot::~ScrollerHotspot()
{
}


void ScrollerHotspot::ScrollToPosition(glm::vec2 position)
{
	glm::mat4 transform;
	for (Container* c = _scroller->GetContainer(); c != nullptr; c = c->GetContainer())
		transform = c->GetContentTransform() * transform;


	glm::vec4 p1 = _scroller->GetContentTransform() * glm::vec4(_position, 0, 1);
	glm::vec4 p2 = glm::inverse(transform) * glm::vec4(position, 0, 1);

	glm::vec2 delta = p2.xy() / p2.w - p1.xy() / p1.w;

	if (!_scroller->GetHorizontalScrolling())
		delta.x = 0;
	if (!_scroller->GetVerticalScrolling())
		delta.y = 0;

	_scroller->SetContentOffset(_scroller->GetContentOffset() - delta);
}
