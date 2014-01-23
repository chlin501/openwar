// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Content.h"
#include "shaderprogram.h"
#include <glm/gtc/matrix_transform.hpp>



glm::mat4 ViewportTransform(bounds2f viewport, glm::vec2 translate, float rotate)
{
	glm::vec2 viewport_center = viewport.center();
	glm::vec2 viewport_scale = 2.0f / viewport.size();

	glm::mat4x4 result = glm::scale(glm::mat4x4(), glm::vec3(viewport_scale.x, viewport_scale.y, 1.0f))
		* glm::translate(glm::mat4x4(), glm::vec3(translate.x - viewport_center.x, translate.y - viewport_center.y, 0.0f));

	if (rotate != 0)
		result = result * glm::rotate(glm::mat4x4(), rotate * 180 / (float)M_PI, glm::vec3(0, 0, 1));

	return result;
}



Content::Content(Surface* surface) :
_surface(surface),
_frame(),
_aspect(0),
_flip(false)
{
}


Content::~Content()
{
}


bounds2f Content::GetFrame() const
{
	return _frame;
}


void Content::SetFrame(bounds2f value)
{
	_frame = value;
}


void Content::UseViewport()
{
	bounds2f viewport = _frame * GetSurface()->GetGraphicsContext()->get_pixeldensity();
	glViewport((GLint)viewport.min.x, (GLint)viewport.min.y, (GLsizei)viewport.size().x, (GLsizei)viewport.size().y);
}


glm::vec2 Content::SurfaceToContent(glm::vec2 value) const
{
	bounds2f viewport = GetFrame();
	return 2.0f * (value - viewport.p11()) / viewport.size() - 1.0f;
}


glm::vec2 Content::ContentToSurface(glm::vec2 value) const
{
	bounds2f viewport = GetFrame();
	return viewport.p11() + (value + 1.0f) / 2.0f * viewport.size();
}


void Content::ScreenSizeChanged()
{
}


glm::mat4 Content::GetRenderTransform() const
{
	return ViewportTransform(bounds2f(0, 0, _surface->GetSize()));
}
