// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/matrix_transform.hpp>
#include "Content.h"
#include "Surface.h"



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
_container(nullptr),
_surface(surface),
_frame(),
_anchor(),
_flip(false)
{
}


Content::Content() :
_container(nullptr),
_surface(nullptr),
_frame(),
_anchor(),
_flip(false)
{
}


Content::~Content()
{
}


Container* Content::GetContainer() const
{
	return _container;
}


void Content::SetContainer(Container* value)
{
	SetContentContainer(this, value);
}


bounds2f Content::GetFrame() const
{
	return _frame;
}


void Content::SetFrame(bounds2f value)
{
	SetFrameValue(value);
}


glm::vec2 Content::GetAnchor() const
{
	return _anchor;
}


void Content::SetAnchor(glm::vec2 value)
{
	_anchor = value;
}


glm::vec2 Content::GetPosition() const
{
	return glm::mix(_frame.min, _frame.max, _anchor);
}


void Content::SetPosition(glm::vec2 value)
{
	glm::vec2 delta = value - GetPosition();
	SetFrameValue(_frame + delta);
}


glm::vec2 Content::GetSize() const
{
	return _frame.size();
}


void Content::SetSize(glm::vec2 value)
{
	glm::vec2 p = GetPosition();
	SetFrameValue(bounds2f(p - value * _anchor,  p + value * (glm::vec2(1, 1) - _anchor)));
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


glm::mat4 Content::GetContentTransform() const
{
	glm::mat4 t;
	t = glm::translate(t, glm::vec3(_frame.min, 0));
	return t;
}


void Content::SetFrameValue(const bounds2f& value)
{
	/*glm::vec2 oldPosition = GetPosition();
	glm::vec2 oldSize = GetSize();
	float oldLeft = GetLeft();
	float oldRight = GetRight();
	float oldBottom = GetBottom();
	float oldTop = GetTop();
	float oldWidth = GetWidth();
	float oldHeight = GetHeight();*/

	_frame = value;

	//if (_container != nullptr)
	//	_container->OnFrameChanged(this);

	/*if (oldPosition != GetPosition())
		position.CallObserver();
	if (oldSize != GetSize())
		size.CallObserver();
	if (oldLeft != GetLeft())
		left.CallObserver();
	if (oldRight != GetRight())
		right.CallObserver();
	if (oldBottom != GetBottom())
		bottom.CallObserver();
	if (oldTop != GetTop())
		top.CallObserver();
	if (oldWidth != GetWidth())
		width.CallObserver();
	if (oldHeight != GetHeight())
		height.CallObserver();*/
}
