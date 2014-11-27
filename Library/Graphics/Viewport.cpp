// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Viewport.h"
#include "GraphicsContext.h"
#include "GraphicsOpenGL.h"


Viewport::Viewport(GraphicsContext* gc) :
	_gc(gc)
{
}


Viewport::~Viewport()
{
}


bounds2i Viewport::GetBounds() const
{
	return _bounds;
}


void Viewport::SetBounds(const bounds2i& value)
{
	_bounds = value;
}


void Viewport::UseViewport() const
{
	bounds2f bounds = (bounds2f)_bounds * _gc->GetCombinedScaling();
	glViewport((GLint)bounds.min.x, (GLint)bounds.min.y, (GLsizei)bounds.x().size(), (GLsizei)bounds.y().size());
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
