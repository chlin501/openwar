#include "WidgetViewport.h"
#include <glm/gtc/matrix_transform.hpp>


WidgetViewport::WidgetViewport(GraphicsContext* gc) : Viewport(gc)
{
}


WidgetViewport::~WidgetViewport()
{
}


glm::mat4 WidgetViewport::GetTransform() const
{
	glm::mat4 result;

	result = glm::translate(result, glm::vec3(-1, -1, 0));
	result = glm::scale(result, glm::vec3(glm::vec2(2, 2) / (glm::vec2)GetBounds().size(), 1));
	result = glm::translate(result, glm::vec3(-_contentOffset, 0));

	return result;
}


glm::vec2 WidgetViewport::GetContentOffset() const
{
	return _contentOffset;
}


void WidgetViewport::SetContentOffset(glm::vec2 value)
{
	_contentOffset = value;
}


static float ClampContentOffset(float value, bounds1f bounds)
{
	return !bounds.empty() ? bounds.clamp(value) : bounds.center();
}


glm::vec2 WidgetViewport::ClampContentOffset(glm::vec2 value) const
{
	bounds2f bounds = bounds2f(0, 0, _contentSize - (glm::vec2)GetBounds().size());
	return glm::vec2(
		::ClampContentOffset(value.x, bounds.x()),
		::ClampContentOffset(value.y, bounds.y()));
}


glm::vec2 WidgetViewport::GetContentSize() const
{
	return _contentSize;
}


void WidgetViewport::SetContentSize(glm::vec2 value)
{
	_contentSize = value;
}


bounds2f WidgetViewport::GetVisibleBounds() const
{
	return bounds2f(0, 0, GetBounds().x().size(), GetBounds().y().size()) + _contentOffset;
}


glm::vec2 WidgetViewport::LocalToGlobal(glm::vec2 value) const
{
	return NormalizedToGlobal(LocalToNormalized(value));
}


glm::vec2 WidgetViewport::GlobalToLocal(glm::vec2 value) const
{
	return NormalizedToLocal(GlobalToNormalized(value));
}


glm::vec2 WidgetViewport::LocalToNormalized(glm::vec2 value) const
{
	return 2.0f * (value - _contentOffset) / (glm::vec2)GetBounds().size() - 1.0f;
}


glm::vec2 WidgetViewport::NormalizedToLocal(glm::vec2 value) const
{
	return _contentOffset + (value + 1.0f) / 2.0f * (glm::vec2)GetBounds().size();
}
