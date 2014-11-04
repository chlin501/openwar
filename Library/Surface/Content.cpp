// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/matrix_transform.hpp>
#include "Content.h"
#include "Surface.h"
#include "CommonShaders.h"
#include "Shapes/VertexShape.h"
#import "WidgetShape.h"



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


bounds2f Content::GetViewport() const
{
	return _viewport;
}


void Content::SetViewport(bounds2f value)
{
	_viewport = value;
}


void Content::UseViewport()
{
	bounds2f viewport;

	for (Content* c = this; c != nullptr && viewport.is_empty(); c = c->GetContainer())
		viewport = c->GetViewport();

	viewport = viewport * GetSurface()->GetGraphicsContext()->GetPixelDensity();
	glViewport((GLint)viewport.min.x, (GLint)viewport.min.y, (GLsizei)viewport.size().x, (GLsizei)viewport.size().y);
}



bounds2f Content::GetFrame() const
{
	return _frame;
}


void Content::SetFrame(bounds2f value)
{
	SetFrameValue(value);
}


void Content::OnFrameChanged()
{
}




glm::vec2 Content::GetPosition() const
{
	return _frame.min;
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
	SetFrameValue(bounds2f(p,  p + value));
}


bounds2f Content::GetBounds() const
{
	return _bounds;
}


void Content::SetBounds(const bounds2f& value)
{
	_bounds = value;
	OnBoundsChanged();
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



glm::mat4 Content::GetViewportTransform() const
{
	bounds2f viewport;

	for (const Content* c = this; c != nullptr && viewport.is_empty(); c = c->GetContainer())
		viewport = c->GetViewport();

	glm::mat4 result;

	if (!viewport.is_empty())
	{
		glm::vec2 size = viewport.size();

		result = glm::translate(result, glm::vec3(-1, -1, 0));
		result = glm::scale(result, glm::vec3(2 / size.x, 2 / size.y, 1));
		result = glm::translate(result, glm::vec3(-viewport.min, 0));
	}

	return result;
}


glm::mat4 Content::GetContainerTransform() const
{
	glm::mat4 result;

	for (const Content* c = GetContainer(); c != nullptr; c = c->GetContainer())
	{
		result = c->GetContentTransform() * result;
	}

	return result;
}


glm::mat4 Content::GetContentTransform() const
{
	bounds2f frame = GetFrame();

	glm::mat4 result;

	result = glm::translate(result, glm::vec3(frame.min, 0));
	result = glm::translate(result, glm::vec3(_translate, 0));

	return result;
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


void Content::SetFrameValue(const bounds2f& value)
{
	_frame = value;

	OnFrameChanged();
}


void Content::OnBoundsChanged()
{

}


void Content::RenderSolid(const glm::mat4& transform, bounds2f bounds, glm::vec4 color) const
{
	VertexShape_2f vertices;
	vertices._mode = GL_TRIANGLE_STRIP;
	vertices.AddVertex(Vertex_2f(bounds.p11()));
	vertices.AddVertex(Vertex_2f(bounds.p12()));
	vertices.AddVertex(Vertex_2f(bounds.p21()));
	vertices.AddVertex(Vertex_2f(bounds.p22()));

	GraphicsContext* gc = GetSurface()->GetGraphicsContext();
	RenderCall<PlainShader_2f>(gc)
		.SetVertices(&vertices)
		.SetUniform("transform", transform)
		.SetUniform("point_size", 1)
		.SetUniform("color", color)
		.Render();
}
