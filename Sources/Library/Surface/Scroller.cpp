#include <glm/gtc/matrix_transform.hpp>
#include "Scroller.h"
#include "ScrollerHotspot.h"


Scroller::Scroller() :
_contentSize(),
_contentOffset(),
_zoomScale(1),
contentSize([this]() { return GetContentSize(); }, [this](glm::vec2 value) { SetContentSize(value); }),
contentOffset([this]() { return GetContentOffset(); }, [this](glm::vec2 value) { SetContentOffset(value); }),
zoomScale([this]() { return GetZoomScale(); }, [this](float value) { SetZoomScale(value); })
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


glm::mat4 Scroller::GetContentTransform() const
{
	glm::mat4 transform = Container::GetContentTransform();

	transform = glm::scale(transform, glm::vec3(_zoomScale, _zoomScale, 1));
	transform = glm::translate(transform, -glm::vec3(_contentOffset, 0));

	return transform;
}


void Scroller::Render(const glm::mat4& transform)
{
	//RenderSolid(transform, GetBounds(), glm::vec4(1.0f, 0.0f, 0.0f, 0.15f));
	Container::Render(transform);
}


void Scroller::Update(double secondsSinceLastUpdate)
{
	Container::Update(secondsSinceLastUpdate);
}


void Scroller::FindHotspots(const glm::mat4 transform, glm::vec2 position, std::function<void(std::shared_ptr<Hotspot>)> action)
{
	Container::FindHotspots(transform, position, action);

	glm::mat4 inverse = glm::inverse(transform);
	glm::vec4 p = inverse * glm::vec4(position, 0, 1);
	glm::vec2 pos = p.xy() / p.w;

	if (GetBounds().contains(pos))
	{
		action(std::make_shared<ScrollerHotspot>(this, pos));
	}
}
