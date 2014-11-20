// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "WidgetView.h"
#include "GraphicsContext.h"
#include "RenderCall.h"
#include "ScrollerHotspot.h"
#include "ScrollerViewport.h"
#include "Surface.h"
#include "TextureAtlas.h"
#include "TextureFont.h"
#include "Touch.h"
#include "Widget.h"
#include "WidgetShader.h"


/* WidgetShape::WidgetVertexBuffer */


WidgetView::WidgetVertexBuffer::WidgetVertexBuffer(WidgetView* widgetView) :
	_widgetView(widgetView)
{
}


void WidgetView::WidgetVertexBuffer::Update()
{
	_widgetView->UpdateVertexBuffer();
}


/* WidgetView */


WidgetView::WidgetView(Surface* surface) : View(surface),
	_gc(surface->GetGraphicsContext()),
	_viewport(nullptr),
	_textureAtlas(nullptr),
	_vertices(this)
{
	_vertices._mode = GL_TRIANGLES;
	_viewport = new ScrollerViewport(_gc);
	_textureAtlas = _gc->GetWidgetTextureAtlas();
}


WidgetView::~WidgetView()
{
}


ScrollerViewport* WidgetView::GetScrollerViewport() const
{
	return _viewport;
}



TextureAtlas* WidgetView::GetTextureAtlas() const
{
	return _textureAtlas;
}


Viewport* WidgetView::GetViewport() const
{
	return _viewport;
}


void WidgetView::SetBounds(const bounds2f& value)
{
	View::SetBounds(value);
	_viewport->SetBounds(value);
}


void WidgetView::OnTouchEnter(Touch* touch)
{
	CallWidgets_OnTouchEnter(touch);
}


void WidgetView::OnTouchBegin(Touch* touch)
{
	if (_viewport->GetContentSize() != glm::vec2(0, 0))
	{
		bounds2f viewportBounds = _viewport->GetBounds();
		if (viewportBounds.contains(touch->GetOriginalPosition()))
		{
			if (_scrollerHotspot == nullptr)
				_scrollerHotspot = std::make_shared<ScrollerHotspot>(_viewport);
			_scrollerHotspot->SubscribeTouch(touch);
		}
	}

	CallWidgets_OnTouchBegin(touch);
}


void WidgetView::Render()
{
	_viewport->UseViewport();

	RenderCall<WidgetShader>(_gc)
		.SetVertices(&_vertices, "position", "texcoord", "colorize", "alpha")
		.SetUniform("transform", _viewport->GetTransform())
		.SetTexture("texture", _textureAtlas)
		.Render();
}


WidgetView* WidgetView::FindWidgetView()
{
	return this;
}


void WidgetView::UpdateVertexBuffer()
{
	static std::vector<Vertex_2f_2f_4f_1f> vertices;

	CallWidgets_AppendVertices(vertices);

	_vertices.UpdateVBO(GL_TRIANGLES, vertices.data(), vertices.size());
	vertices.clear();
}
