// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "WidgetView.h"

#include "Graphics/RenderCall.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/TextureFont.h"
#include "Surface/Surface.h"
#include "Surface/Touch.h"
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


WidgetView::WidgetView(ViewOwner* viewOwner, std::shared_ptr<Viewport2D> viewport) : View(viewOwner, viewport),
	_gc{GetGraphicsContext()},
	_viewport2D(*viewport),
	_textureAtlas(_gc->GetTextureAtlas(WIDGET_TEXTURE_ATLAS)),
	_vertices(this)
{
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
	CallWidgets_OnTouchBegin(touch);
}


void WidgetView::Render()
{
	RenderCall<WidgetShader>(_gc)
		.SetVertices(&_vertices, "position", "texcoord", "colorize", "alpha")
		.SetUniform("transform", _viewport2D.GetTransform())
		.SetTexture("texture", _textureAtlas)
		.Render(_viewport2D);
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


/* ScrollerWidgetView */


#ifdef OPENWAR_ENABLE_LEGACY_UI
ScrollerWidgetView::ScrollerWidgetView(ViewOwner* viewOwner, std::shared_ptr<ScrollerViewport> viewport) : WidgetView{viewOwner, viewport},
	_scrollerHotspot{*viewport}
{
}


void ScrollerWidgetView::OnTouchEnter(Touch* touch)
{
	WidgetView::OnTouchEnter(touch);
}


void ScrollerWidgetView::OnTouchBegin(Touch* touch)
{
	WidgetView::OnTouchBegin(touch);

	if (!touch->HasSubscribers())
	{
		if (GetViewport2D().GetContentSize() != glm::vec2{})
		{
			bounds2f viewportBounds = GetViewport2D().GetViewportBounds();
			if (viewportBounds.contains(touch->GetOriginalPosition()))
			{
				_scrollerHotspot.SubscribeTouch(touch);
			}
		}
	}
}
#endif
