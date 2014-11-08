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
	_viewportBounds(),
	_contentOffset(),
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



bounds2i Content::GetViewportBounds() const
{
	return _viewportBounds;
}


void Content::SetViewportBounds(bounds2i value)
{
	_viewportBounds = value;
}


void Content::UseViewport()
{
	bounds2f bounds = (bounds2f)_viewportBounds * _gc->GetPixelDensity();
	glViewport((GLint)bounds.min.x, (GLint)bounds.min.y, (GLsizei)bounds.x().size(), (GLsizei)bounds.y().size());
}


glm::vec2 Content::GetContentOffset() const
{
	return _contentOffset;
}


void Content::SetContentOffset(glm::vec2 value)
{
	_contentOffset = value;
}


static float ClampContentOffset(float value, bounds1f bounds)
{
	return !bounds.empty() ? bounds.clamp(value) : bounds.center();
}


glm::vec2 Content::ClampContentOffset(glm::vec2 value) const
{
	bounds2f bounds = bounds2f(0, 0, _contentSize - (glm::vec2)_viewportBounds.size());
	return glm::vec2(
		::ClampContentOffset(value.x, bounds.x()),
		::ClampContentOffset(value.y, bounds.y()));
}


glm::vec2 Content::GetContentSize() const
{
	return _contentSize;
}


void Content::SetContentSize(glm::vec2 value)
{
	_contentSize = value;
}


bounds2f Content::GetVisibleBounds() const
{
	return bounds2f(0, 0, _viewportBounds.x().size(), _viewportBounds.y().size()) + _contentOffset;
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
	result = glm::scale(result, glm::vec3(glm::vec2(2, 2) / (glm::vec2)_viewportBounds.size(), 1));
	result = glm::translate(result, glm::vec3(-_contentOffset, 0));

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
	return 2.0f * (value - _contentOffset) / (glm::vec2)_viewportBounds.size() - 1.0f;
}


glm::vec2 Content::NormalizedToContent(glm::vec2 value) const
{
	return _contentOffset + (value + 1.0f) / 2.0f * (glm::vec2)_viewportBounds.size();
}


glm::vec2 Content::ViewportToNormalized(glm::vec2 value) const
{
	return 2.0f * (value - (glm::vec2)_viewportBounds.min) / (glm::vec2)_viewportBounds.size() - 1.0f;
}


glm::vec2 Content::NormalizedToViewport(glm::vec2 value) const
{
	return (glm::vec2)_viewportBounds.min + (value + 1.0f) / 2.0f * (glm::vec2)_viewportBounds.size();
}
