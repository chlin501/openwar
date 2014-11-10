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


void Viewport::UseViewport()
{
	bounds2f bounds = (bounds2f)_bounds * _gc->GetPixelDensity();
	glViewport((GLint)bounds.min.x, (GLint)bounds.min.y, (GLsizei)bounds.x().size(), (GLsizei)bounds.y().size());
}


glm::vec2 Viewport::GlobalToNormalized(glm::vec2 value) const
{
	return 2.0f * (value - (glm::vec2)_bounds.min) / (glm::vec2)_bounds.size() - 1.0f;
}


glm::vec2 Viewport::NormalizedToGlobal(glm::vec2 value) const
{
	return (glm::vec2)_bounds.min + (value + 1.0f) / 2.0f * (glm::vec2)_bounds.size();
}
