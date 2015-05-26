#include "ScrollerView.h"
#include "Touch.h"


ScrollerView::ScrollerView(ViewOwner* viewOwner, std::shared_ptr<ScrollerViewport> viewport) : View{viewOwner, viewport},
	_scrollerViewport{viewport},
	_scrollerHotspot{*viewport}
{
}


bounds2f ScrollerView::GetCoverage() const
{
	glm::vec2 viewportSize = _scrollerViewport->GetViewportBounds().size();
	glm::vec2 contentSize = _scrollerViewport->GetContentSize();
	glm::vec2 offset = _scrollerViewport->GetContentOffset();

	bounds1f content_x{0, contentSize.x};
	bounds1f content_y{0, contentSize.y};

	return bounds2f{
		content_x.unmix(offset.x),
		content_y.unmix(offset.y),
		content_x.unmix(offset.x + viewportSize.x),
		content_y.unmix(offset.y + viewportSize.y),
	};
}


void ScrollerView::OnTouchEnter(Touch* touch)
{
}


void ScrollerView::OnTouchBegin(Touch* touch)
{
	if (_scrollerViewport->GetContentSize() != glm::vec2{})
	{
		bounds2f viewportBounds = _scrollerViewport->GetViewportBounds();
		if (viewportBounds.contains(touch->GetOriginalPosition()))
		{
			_scrollerHotspot.SubscribeTouch(touch);
		}
	}
}


void ScrollerView::Render()
{
}
