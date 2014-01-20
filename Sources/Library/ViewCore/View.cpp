// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "View.h"
#include "../Graphics/renderer.h"
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



View::View(Surface* surface) :
_surface(surface),
_viewport(),
_aspect(0),
_flip(false)
{
	UpdateAspect();
}


View::~View()
{
}


bounds2f View::GetViewportBounds() const
{
	return _viewport.is_empty() ? bounds2f(0, 0, _surface->GetSize()) : _viewport;
}


void View::SetViewport(bounds2f value)
{
	_viewport = value;
	UpdateAspect();
}


void View::UseViewport()
{
	if (_viewport.is_empty())
	{
		_surface->UseViewport();
	}
	else
	{
		bounds2f viewport = _viewport * GetSurface()->GetPixelDensity();
		glViewport((GLint)viewport.min.x, (GLint)viewport.min.y, (GLsizei)viewport.size().x, (GLsizei)viewport.size().y);
	}
}


glm::vec2 View::ScreenToView(glm::vec2 value) const
{
	bounds2f viewport = GetViewportBounds();
	return 2.0f * (value - viewport.p11()) / viewport.size() - 1.0f;
}


glm::vec2 View::ViewToScreen(glm::vec2 value) const
{
	bounds2f viewport = GetViewportBounds();
	return viewport.p11() + (value + 1.0f) / 2.0f * viewport.size();
}


void View::ScreenSizeChanged()
{
	UpdateAspect();
}


/*void View::Update(double secondsSinceLastUpdate)
{
}*/



void View::UpdateAspect()
{
	if (_viewport.is_empty())
		_aspect = _surface->GetSize().y / _surface->GetSize().x;
	else
		_aspect = _viewport.size().y / _viewport.size().x;
}
