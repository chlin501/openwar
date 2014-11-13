// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "WidgetView.h"
#include "GraphicsContext.h"
#include "RenderCall.h"
#include "ScrollerViewport.h"
#include "StringWidget.h"
#include "Widget.h"
#include "WidgetShader.h"
#include "TextureAtlas.h"
#include "TextureFont.h"


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


WidgetView::WidgetView(GraphicsContext* gc) :
	_gc(gc),
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


ScrollerViewport* WidgetView::GetViewport() const
{
	return _viewport;
}



TextureAtlas* WidgetView::GetTextureAtlas() const
{
	return _textureAtlas;
}


glm::vec2 WidgetView::MeasureStringWidget(StringWidget* stringWidget) const
{
	TextureFont* textureFont = _textureAtlas->GetTextureFont(stringWidget->GetFontDescriptor());

	return textureFont->MeasureText(stringWidget->GetString());
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


WidgetView* WidgetView::GetWidgetView()
{
	return this;
}


void WidgetView::UpdateVertexBuffer()
{
	static std::vector<Vertex_2f_2f_4f_1f> vertices;

	for (Widget* widget : GetWidgets())
		if (widget->IsVisible())
			widget->AppendVertices(vertices);

	_vertices.UpdateVBO(GL_TRIANGLES, vertices.data(), vertices.size());
	vertices.clear();
}
