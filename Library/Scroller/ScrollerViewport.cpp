// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ScrollerViewport.h"
#include <glm/gtc/matrix_transform.hpp>


ScrollerViewport::ScrollerViewport(GraphicsContext* gc) : Viewport(gc)
{
}


ScrollerViewport::~ScrollerViewport()
{
}


glm::mat4 ScrollerViewport::GetTransform() const
{
	glm::mat4 result;

	result = glm::translate(result, glm::vec3(-1, -1, 0));
	result = glm::scale(result, glm::vec3(glm::vec2(2, 2) / (glm::vec2)GetViewportBounds().size(), 1));
	result = glm::translate(result, glm::vec3(-_contentOffset, 0));

	return result;
}


glm::vec2 ScrollerViewport::GetContentOffset() const
{
	return _contentOffset;
}


void ScrollerViewport::SetContentOffset(glm::vec2 value)
{
	_contentOffset = value;
}


static float ClampContentOffset(float value, float contentSize, bounds1i viewportBounds)
{
	if (contentSize == 0)
		return 0;

	bounds1f bounds = bounds1f(0, contentSize - viewportBounds.size());
	return !viewportBounds.empty() ? bounds.clamp(value) : bounds.mid();
}


glm::vec2 ScrollerViewport::GetClampedOffset(glm::vec2 value) const
{
	bounds2i viewportBounds = GetViewportBounds();
	return glm::vec2(
		::ClampContentOffset(value.x, _contentSize.x, viewportBounds.x()),
		::ClampContentOffset(value.y, _contentSize.y, viewportBounds.y()));
}


void ScrollerViewport::ClampContentOffset()
{
	_contentOffset = GetClampedOffset(_contentOffset);
}


glm::vec2 ScrollerViewport::GetContentSize() const
{
	return _contentSize;
}


void ScrollerViewport::SetContentSize(glm::vec2 value)
{
	_contentSize = value;
}


bounds2f ScrollerViewport::GetVisibleBounds() const
{
	bounds2i bounds = GetViewportBounds();
	return bounds2f(0, 0, bounds.x().size(), bounds.y().size()) + _contentOffset;
}


glm::vec2 ScrollerViewport::LocalToGlobal(glm::vec2 value) const
{
	return NormalizedToGlobal(LocalToNormalized(value));
}


glm::vec2 ScrollerViewport::GlobalToLocal(glm::vec2 value) const
{
	return NormalizedToLocal(GlobalToNormalized(value));
}


glm::vec2 ScrollerViewport::LocalToNormalized(glm::vec2 value) const
{
	glm::ivec2 size = GetViewportBounds().size();
	if (size.x == 0 || size.y == 0)
		return glm::vec2(-1, -1);

	return 2.0f * (value - _contentOffset) / (glm::vec2)size - 1.0f;
}


glm::vec2 ScrollerViewport::NormalizedToLocal(glm::vec2 value) const
{
	glm::ivec2 size = GetViewportBounds().size();
	if (size.x == 0 || size.y == 0)
		return glm::vec2(0, 0);

	return _contentOffset + (value + 1.0f) / 2.0f * (glm::vec2)size;
}
