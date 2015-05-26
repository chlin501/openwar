// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Viewport.h"
#include "GraphicsContext.h"
#include "FrameBuffer.h"
#include <glm/gtc/matrix_transform.hpp>


Viewport::Viewport(GraphicsContext* gc) :
	_gc{gc}
{
}


Viewport::~Viewport()
{
}


GraphicsContext* Viewport::GetGraphicsContext() const
{
	return _gc;
}


FrameBuffer* Viewport::GetFrameBuffer() const
{
	return _frameBuffer;
}


void Viewport::SetFrameBuffer(FrameBuffer* frameBuffer)
{
	_frameBuffer = frameBuffer;
}


bounds2i Viewport::GetViewportBounds() const
{
	return _bounds;
}


void Viewport::SetViewportBounds(const bounds2i& value)
{
	_bounds = value;
}


glm::vec2 Viewport::ScreenToNormalized(glm::vec2 value) const
{
	glm::ivec2 size = _bounds.size();
	if (size.x == 0 || size.y == 0)
		return glm::vec2(-1, -1);

	return 2.0f * (value - (glm::vec2)_bounds.min) / (glm::vec2)size - 1.0f;
}


glm::vec2 Viewport::NormalizedToScreen(glm::vec2 value) const
{
	glm::ivec2 size = _bounds.size();
	if (size.x == 0 || size.y == 0)
		return glm::vec2(0, 0);

	return (glm::vec2)_bounds.min + (value + 1.0f) / 2.0f * (glm::vec2)_bounds.size();
}


/* Viewport2D */


Viewport2D::Viewport2D(GraphicsContext* gc) : Viewport{gc}
{

}


glm::vec2 Viewport2D::ContentToNormalized(glm::vec2 value) const
{
	return ScreenToNormalized(ContentToScreen(value));
}


glm::vec2 Viewport2D::NormalizedToContent(glm::vec2 value) const
{
	return ScreenToContent(NormalizedToScreen(value));
}



/***/


StandardViewport2D::StandardViewport2D(GraphicsContext* gc) : Viewport2D{gc}
{
}


glm::vec2 StandardViewport2D::GetContentSize() const
{
	return (glm::vec2)GetViewportBounds().size();
}


bounds2f StandardViewport2D::GetVisibleBounds() const
{
	return bounds2f{0, 0, (glm::vec2)GetViewportBounds().size()};
}


glm::vec2 StandardViewport2D::ContentToScreen(glm::vec2 value) const
{
	return value + (glm::vec2)GetViewportBounds().min;
}


glm::vec2 StandardViewport2D::ScreenToContent(glm::vec2 value) const
{
	return value - (glm::vec2)GetViewportBounds().min;
}


glm::mat4 StandardViewport2D::GetTransform() const
{
	glm::mat4 result;
	bounds2i bounds = GetViewportBounds();

	result = glm::translate(result, glm::vec3{-1, -1, 0});
	result = glm::scale(result, glm::vec3{glm::vec2{2, 2} / (glm::vec2)bounds.size(), 1});

	return result;
}


/* ScrollerViewport */



ScrollableViewport2D::ScrollableViewport2D(GraphicsContext* gc) : Viewport2D{gc}
{
}


void ScrollableViewport2D::SetContentSize(glm::vec2 value)
{
	_contentSize = value;
}


glm::vec2 ScrollableViewport2D::GetContentOffset() const
{
	return _contentOffset;
}


void ScrollableViewport2D::SetContentOffset(glm::vec2 value)
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


glm::vec2 ScrollableViewport2D::GetClampedOffset(glm::vec2 value) const
{
	bounds2i viewportBounds = GetViewportBounds();
	return glm::vec2(
		::ClampContentOffset(value.x, _contentSize.x, viewportBounds.x()),
		::ClampContentOffset(value.y, _contentSize.y, viewportBounds.y()));
}


void ScrollableViewport2D::ClampContentOffset()
{
	_contentOffset = GetClampedOffset(_contentOffset);
}


glm::vec2 ScrollableViewport2D::GetContentSize() const
{
	return _contentSize;
}


bounds2f ScrollableViewport2D::GetVisibleBounds() const
{
	bounds2i bounds = GetViewportBounds();
	return bounds2f(0, 0, bounds.x().size(), bounds.y().size()) + _contentOffset;
}


glm::vec2 ScrollableViewport2D::ContentToScreen(glm::vec2 value) const
{
	return value + (glm::vec2)GetViewportBounds().min - _contentOffset;
}


glm::vec2 ScrollableViewport2D::ScreenToContent(glm::vec2 value) const
{
	return value - (glm::vec2)GetViewportBounds().min + _contentOffset;
}


glm::mat4 ScrollableViewport2D::GetTransform() const
{
	glm::mat4 result;

	result = glm::translate(result, glm::vec3(-1, -1, 0));
	result = glm::scale(result, glm::vec3(glm::vec2(2, 2) / (glm::vec2)GetViewportBounds().size(), 1));
	result = glm::translate(result, glm::vec3(-_contentOffset, 0));

	return result;
}
