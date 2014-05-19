#include "Scroller.h"
#include "ScrollerGesture.h"
#include "ScrollerHotspot.h"
#include "Touch.h"
#include "Surface.h"


ScrollerGesture::ScrollerGesture(Surface* surface) :
_surface(surface),
_scroller(nullptr)
{
}


ScrollerGesture::~ScrollerGesture()
{
}


void ScrollerGesture::Update(Surface* surface, double secondsSinceLastUpdate)
{

}


void ScrollerGesture::TouchBegan(Touch* touch)
{
	if (/*touch->HasGesture() ||*/ !_touches.empty())
		return;

	if (_scroller != nullptr)
		return;

	std::shared_ptr<ScrollerHotspot> scrollerHotspot;

	_surface->FindHotspots(glm::mat4(), touch->GetPosition(), [this, &scrollerHotspot](std::shared_ptr<Hotspot> hotspot) {
		if (scrollerHotspot == nullptr)
			scrollerHotspot = std::dynamic_pointer_cast<ScrollerHotspot>(hotspot);
	});

	if (scrollerHotspot != nullptr)
	{
		CaptureTouch(touch);

		_scroller = scrollerHotspot->GetScroller();
		_position = scrollerHotspot->GetPosition();
	}
}


void ScrollerGesture::TouchMoved()
{
	if (_scroller != nullptr && !_touches.empty())
	{
		glm::mat4 transform;
		for (Container* c = _scroller->GetContainer(); c != nullptr; c = c->GetContainer())
			transform = c->GetContentTransform() * transform;


		glm::vec4 p1 = _scroller->GetContentTransform() * glm::vec4(_position, 0, 1);
		glm::vec4 p2 = glm::inverse(transform) * glm::vec4(_touches.front()->GetPosition(), 0, 1);

		glm::vec2 delta = p2.xy() / p2.w - p1.xy() / p1.w;

		if (!_scroller->GetHorizontalScrolling())
			delta.x = 0;
		if (!_scroller->GetVerticalScrolling())
			delta.y = 0;

		_scroller->SetContentOffset(_scroller->GetContentOffset() - delta);
	}
}


void ScrollerGesture::TouchEnded(Touch* touch)
{
	if (_scroller != nullptr)
	{
		_scroller = nullptr;
	}
}
