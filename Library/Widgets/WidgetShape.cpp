// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "WidgetShape.h"
#include "GraphicsContext.h"
#include "TextureAtlas.h"
#include "TextureFont.h"
#include "Widget.h"
#include "StringWidget.h"


/* WidgetShape::WidgetVertexBuffer */


WidgetShape::WidgetVertexBuffer::WidgetVertexBuffer(WidgetShape* shape) :
	_widgetShape(shape)
{
}


void WidgetShape::WidgetVertexBuffer::Update()
{
	_widgetShape->UpdateVertexBuffer();
}


/* WidgetShape */



WidgetShape::WidgetShape(TextureAtlas* textureAtlas) :
	_textureAtlas(textureAtlas),
	_vertices(this)
{
	_vertices._mode = GL_TRIANGLES;
}


WidgetShape::~WidgetShape()
{
	for (Widget* widget : _widgets)
		widget->_widgetShape = nullptr;
}


TextureAtlas* WidgetShape::GetTextureAtlas() const
{
	return _textureAtlas;
}


VertexBuffer<Vertex_2f_2f_4f_1f>* WidgetShape::GetVertices()
{
	return &_vertices;
}


void WidgetShape::ClearWidgets()
{
	for (Widget* widget : _widgets)
		widget->_widgetShape = nullptr;
	_widgets.clear();
}


void WidgetShape::AddWidget(Widget* widget)
{
	if (widget->_widgetShape != nullptr)
		widget->_widgetShape->RemoveWidget(widget);

	widget->_widgetShape = this;
	_widgets.push_back(widget);
}


void WidgetShape::RemoveWidget(Widget* widget)
{
	widget->_widgetShape = nullptr;
	_widgets.erase(
		std::remove(_widgets.begin(), _widgets.end(), widget),
		_widgets.end());
}


glm::vec2 WidgetShape::MeasureStringWidget(StringWidget* stringWidget) const
{
	TextureFont* textureFont = _textureAtlas->GetTextureFont(stringWidget->GetFontDescriptor());

	return textureFont->MeasureText(stringWidget->GetString());
}


void WidgetShape::UpdateVertexBuffer()
{
	static std::vector<Vertex_2f_2f_4f_1f> vertices;

	for (Widget* widget : _widgets)
		widget->AppendVertices(vertices);

	_vertices.UpdateVBO(GL_TRIANGLES, vertices.data(), vertices.size());
	vertices.clear();
}
