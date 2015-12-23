#include "ScrollbarHotspot.h"


ScrollbarHotspot::ScrollbarHotspot(ScrollerViewport& viewport, ScrollbarDirection direction) :
	_viewport(viewport),
	_direction{direction}
{
}


bounds2f ScrollbarHotspot::GetBounds() const
{
	return _bounds;
}


void ScrollbarHotspot::SetBounds(const bounds2f& value)
{
	_bounds = value;
}


bool ScrollbarHotspot::ShowThumb() const
{
	float viewportSize;
	float contentSize;
	if (_direction == ScrollbarDirection::Horizontal)
	{
		viewportSize = _viewport.GetViewportBounds().x().size();
		contentSize = _viewport.GetContentSize().x;
	}
	else
	{
		viewportSize = _viewport.GetViewportBounds().y().size();
		contentSize = _viewport.GetContentSize().y;
	}
	return contentSize > viewportSize;
}


bounds2f ScrollbarHotspot::GetThumbBounds() const
{
	if (_direction == ScrollbarDirection::Horizontal)
	{
		float viewportSize = _viewport.GetViewportBounds().x().size();
		float contentOffset = _viewport.GetContentOffset().x;
		bounds1f content{0, _viewport.GetContentSize().x};
		bounds1f bounds_x = _bounds.x();
		float x1 = bounds_x.mix(content.unmix(contentOffset));
		float x2 = bounds_x.mix(content.unmix(contentOffset + viewportSize));

		return {x1, x2, _bounds.y()};
	}
	else
	{
		float viewportSize = _viewport.GetViewportBounds().y().size();
		float contentOffset = _viewport.GetContentOffset().y;
		bounds1f content{0, _viewport.GetContentSize().y};
		bounds1f bounds_y = _bounds.y();
		float y1 = bounds_y.mix(content.unmix(contentOffset));
		float y2 = bounds_y.mix(content.unmix(contentOffset + viewportSize));

		return {_bounds.x(), y1, y2};
	}
}


Gesture* ScrollbarHotspot::GetGesture()
{
	return &_gesture;
}
