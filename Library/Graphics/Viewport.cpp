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


glm::vec2 Viewport::GlobalToNormalized(glm::vec2 value) const
{
	glm::ivec2 size = _bounds.size();
	if (size.x == 0 || size.y == 0)
		return glm::vec2(-1, -1);

	return 2.0f * (value - (glm::vec2)_bounds.min) / (glm::vec2)size - 1.0f;
}


glm::vec2 Viewport::NormalizedToGlobal(glm::vec2 value) const
{
	glm::ivec2 size = _bounds.size();
	if (size.x == 0 || size.y == 0)
		return glm::vec2(0, 0);

	return (glm::vec2)_bounds.min + (value + 1.0f) / 2.0f * (glm::vec2)_bounds.size();
}


/***/


BasicViewport::BasicViewport(GraphicsContext* gc) : Viewport{gc}
{
}


glm::mat4 BasicViewport::GetTransform() const
{
	glm::mat4 result;

	result = glm::translate(result, glm::vec3{-1, -1, 0});
	result = glm::scale(result, glm::vec3{glm::vec2{2, 2} / (glm::vec2)GetViewportBounds().size(), 1});

	return result;
}
