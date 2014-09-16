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


void ScrollerHotspot::OnScrollToPosition(std::function<void(glm::vec2)> action)
{
	_action = action;
}


glm::vec2 ScrollerHotspot::GetPosition() const
{
	return _position;
}


void ScrollerHotspot::ScrollToPosition(glm::vec2 position)
{
	if (_action)
		_action(position);

	if (_scroller != nullptr)
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
}
