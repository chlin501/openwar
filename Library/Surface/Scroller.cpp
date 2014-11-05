#include <glm/gtc/matrix_transform.hpp>
#include "Scroller.h"
#include "ScrollerHotspot.h"
#include "Surface.h"
#include "Touch.h"


Scroller::Scroller() :
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


glm::mat4 Scroller::GetContentTransform() const
{
	glm::mat4 transform = Container::GetContentTransform();

	transform = glm::scale(transform, glm::vec3(_zoomScale, _zoomScale, 1));
	transform = glm::translate(transform, -glm::vec3(_contentOffset, 0));

	return transform;
}


void Scroller::FindHotspots(const glm::mat4 transform, glm::vec2 position, Touch* touch)
{
	Container::FindHotspots(transform, position, touch);

	glm::mat4 inverse = glm::inverse(transform);
	glm::vec4 p = inverse * glm::vec4(position, 0, 1);
	glm::vec2 pos = p.xy() / p.w;

	if (GetBounds().contains(pos))
	{
		touch->AddHotspot(std::make_shared<ScrollerHotspot>(pos, this));
	}
}
