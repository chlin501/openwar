// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/matrix_transform.hpp>
#include "Content.h"
#include "Surface.h"
#include "CommonShaders.h"
#include "Shapes/VertexShape.h"
#import "WidgetShape.h"




Content::Content(GraphicsContext* gc) :
	_gc(gc),
	_container(nullptr),
	_visible(true),
	_viewport(),
	_bounds(),
	_isUsingDepth(false),
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


WidgetShape* Content::GetWidgetShape() const
{
	if (_widgetShape == nullptr)
		_widgetShape = new WidgetShape(GetGraphicsContext()->GetWidgetTextureAtlas());

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



bounds2i Content::GetViewport() const
{
	return _viewport;
}


void Content::SetViewport(bounds2i value)
{
	_viewport = value;
}


void Content::UseViewport()
{
	glViewport((GLint)_viewport.min.x, (GLint)_viewport.min.y, (GLsizei)_viewport.x().size(), (GLsizei)_viewport.y().size());
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


glm::mat4 Content::GetRenderTransform() const
{
	glm::mat4 result;

	result = glm::translate(result, glm::vec3(-1, -1, 0));
	result = glm::scale(result, glm::vec3(glm::vec2(2, 2) / _bounds.size(), 1));
	result = glm::translate(result, glm::vec3(-_bounds.min, 0));

	return result;
}


glm::vec2 Content::ContentToViewport(glm::vec2 value) const
{
	return NormalizedToViewport(ContentToNormalized(value));
}


glm::vec2 Content::ViewportToContent(glm::vec2 value) const
{
	return NormalizedToContent(ViewportToNormalized(value));
}


glm::vec2 Content::ContentToNormalized(glm::vec2 value) const
{
	return 2.0f * (value - _bounds.min) / _bounds.size() - 1.0f;
}


glm::vec2 Content::NormalizedToContent(glm::vec2 value) const
{
	return _bounds.min + (value + 1.0f) / 2.0f * _bounds.size();
}


glm::vec2 Content::ViewportToNormalized(glm::vec2 value) const
{
	return 2.0f * (value - (glm::vec2)_viewport.min) / (glm::vec2)_viewport.size() - 1.0f;
}


glm::vec2 Content::NormalizedToViewport(glm::vec2 value) const
{
	return (glm::vec2)_viewport.min + (value + 1.0f) / 2.0f * (glm::vec2)_viewport.size();
}
