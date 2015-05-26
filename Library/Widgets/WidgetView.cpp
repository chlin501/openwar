// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "WidgetView.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/RenderCall.h"
#include "Surface/Surface.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/TextureFont.h"
#include "Surface/Touch.h"
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


WidgetView::WidgetView(ViewOwner* viewOwner, std::shared_ptr<ScrollerViewport> viewport) : View(viewOwner, viewport),
	_gc{GetGraphicsContext()},
	_viewport2D{viewport.get()},
	_scrollerHotspot{viewport.get()},
	_textureAtlas(nullptr),
	_vertices(this)
{
	_vertices._mode = GL_TRIANGLES;
	_textureAtlas = _gc->GetTextureAtlas(WIDGET_TEXTURE_ATLAS);
}


WidgetView::~WidgetView()
{
}


ScrollerViewport* WidgetView::GetScrollerViewport()
{
	return _viewport2D;
}



TextureAtlas* WidgetView::GetWidgetTextureAtlas() const
{
	return _textureAtlas;
}


void WidgetView::OnTouchEnter(Touch* touch)
{
	CallWidgets_OnTouchEnter(touch);
}


void WidgetView::OnTouchBegin(Touch* touch)
{
	if (_viewport2D->GetContentSize() != glm::vec2(0, 0))
	{
		bounds2f viewportBounds = _viewport2D->GetViewportBounds();
		if (viewportBounds.contains(touch->GetOriginalPosition()))
		{
			_scrollerHotspot.SubscribeTouch(touch);
		}
	}

	CallWidgets_OnTouchBegin(touch);
}


void WidgetView::Render()
{
	RenderCall<WidgetShader>(_gc)
		.SetVertices(&_vertices, "position", "texcoord", "colorize", "alpha")
		.SetUniform("transform", _viewport2D->GetTransform())
		.SetTexture("texture", _textureAtlas)
		.Render(*_viewport2D);
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
