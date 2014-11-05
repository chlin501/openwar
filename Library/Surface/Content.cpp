// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/matrix_transform.hpp>
#include "Content.h"
#include "Surface.h"
#include "CommonShaders.h"
#include "Shapes/VertexShape.h"
#import "WidgetShape.h"



glm::mat4 ViewportTransform(bounds2i viewport, glm::vec2 translate, float rotate)
{
	glm::vec2 viewport_center = (glm::vec2)viewport.center();
	glm::vec2 viewport_scale = 2.0f / (glm::vec2)viewport.size();

	glm::mat4x4 result = glm::scale(glm::mat4x4(), glm::vec3(viewport_scale.x, viewport_scale.y, 1.0f))
		* glm::translate(glm::mat4x4(), glm::vec3(translate.x - viewport_center.x, translate.y - viewport_center.y, 0.0f));

	if (rotate != 0)
		result = result * glm::rotate(glm::mat4x4(), rotate * 180 / (float)M_PI, glm::vec3(0, 0, 1));

	return result;
}



Content::Content() :
	_surface(nullptr),
	_container(nullptr),
	_visible(true),
	_frame(),
	_bounds(),
	_isUsingDepth(false),
	_translate(),
	_flip(false),
	_dismissed(false),
	_widgetShape(nullptr)
{
}


Content::~Content()
{
	SetContainer(nullptr, nullptr);
}


void Content::Dismiss()
{
	_dismissed = true;
}


bool Content::IsDismissed() const
{
	return _dismissed;
}


Surface* Content::GetSurface() const
{
	if (_surface == nullptr && _container != nullptr)
		_surface = _container->GetSurface();
	return _surface;
}


WidgetShape* Content::GetWidgetShape() const
{
	if (_widgetShape == nullptr)
	{
		Surface* surface = GetSurface();
		if (surface != nullptr)
			_widgetShape = new WidgetShape(surface->GetGraphicsContext()->GetWidgetTextureAtlas());
	}
	return _widgetShape;
}


Container* Content::GetContainer() const
{
	return _container;
}


void Content::SetContainer(Container* value, Content* behindContent)
{
	SetContentContainer(this, value, behindContent);
}


bool Content::IsVisible() const
{
	return _visible;
}


void Content::SetVisible(bool value)
{
	_visible = value;
}


void Content::UseViewport()
{
	glViewport((GLint)_frame.min.x, (GLint)_frame.min.y, (GLsizei)_frame.x().size(), (GLsizei)_frame.y().size());
}



bounds2i Content::GetFrame() const
{
	return _frame;
}


void Content::SetFrame(bounds2i value)
{
	_frame = value;
}


bounds2f Content::GetBounds() const
{
	return _bounds;
}


void Content::SetBounds(const bounds2f& value)
{
	_bounds = value;
}


bool Content::IsUsingDepth() const
{
	return _isUsingDepth;
}


void Content::SetUsingDepth(bool value)
{
	_isUsingDepth = value;
}


glm::vec2 Content::GetTranslate() const
{
	return _translate;
}


void Content::SetTranslate(glm::vec2 value)
{
	_translate = value;
}



glm::mat4 Content::GetRenderTransform() const
{
	glm::mat4 result;

	result = glm::translate(result, glm::vec3(-1, -1, 0));
	result = glm::scale(result, glm::vec3(glm::vec2(2, 2) / _bounds.size(), 1));
	result = glm::translate(result, glm::vec3(-_bounds.min, 0));

	return result;
}


glm::mat4 Content::GetContentTransform() const
{
	glm::mat4 result;

	result = glm::translate(result, glm::vec3(_bounds.min, 0));
	result = glm::translate(result, glm::vec3(_translate, 0));

	return result;
}


glm::vec2 Content::ConvertContentCoordinateToNormalizedDeviceCoordinate(glm::vec2 value) const
{
	return 2.0f * (value - _bounds.min) / _bounds.size() - 1.0f;
}


glm::vec2 Content::ConvertNormalizedDeviceCoordinateToContentCoordinate(glm::vec2 value) const
{
	return _bounds.min + (value + 1.0f) / 2.0f * _bounds.size();
}
