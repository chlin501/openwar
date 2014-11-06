#include <glm/gtc/matrix_transform.hpp>
#include "Scroller.h"
#include "ScrollerHotspot.h"
#include "Surface.h"
#include "Touch.h"


Scroller::Scroller(GraphicsContext* gc) : Container(gc),
_contentSize(),
_contentOffset(),
_zoomScale(1),
contentSize([this]() { return GetContentSize(); }, [this](glm::vec2 value) { SetContentSize(value); }),
contentOffset([this]() { return GetContentOffset(); }, [this](glm::vec2 value) { SetContentOffset(value); }),
zoomScale([this]() { return GetZoomScale(); }, [this](float value) { SetZoomScale(value); }),
_verticalScrolling(true),
_horizontalScrolling(true)
{
}


Scroller::~Scroller()
{
}


glm::vec2 Scroller::GetContentSize() const
{
	return _contentSize;
}


void Scroller::SetContentSize(glm::vec2 value)
{
	_contentSize = value;
}


glm::vec2 Scroller::GetContentOffset() const
{
	return _contentOffset;
}


void Scroller::SetContentOffset(glm::vec2 value)
{
	_contentOffset = value;
}


float Scroller::GetZoomScale() const
{
	return _zoomScale;
}


void Scroller::SetZoomScale(float value)
{
	_zoomScale = value;
}


bool Scroller::GetVerticalScrolling() const
{
	return _verticalScrolling;
}


void Scroller::SetVerticalScrolling(bool value)
{
	_verticalScrolling = value;
}


bool Scroller::GetHorizontalScrolling() const
{
	return _horizontalScrolling;
}


void Scroller::SetHorizontalScrolling(bool value)
{
	_horizontalScrolling = value;
}


void Scroller::FindHotspots(Touch* touch)
{
	Container::FindHotspots(touch);
/*
	glm::vec2 position = ViewportToContent(touch->GetOriginal());
	if (GetContentBounds().contains(position))
	{
		touch->AddHotspot(std::make_shared<ScrollerHotspot>(position, this));
	}
*/
}
