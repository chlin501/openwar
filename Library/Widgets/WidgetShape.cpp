// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "WidgetShape.h"
#include "GraphicsContext.h"
//#include <glm/gtc/matrix_transform.hpp>


/* WidgetShader */


WidgetShader::WidgetShader(GraphicsContext* gc) : ShaderProgram(
	VERTEX_SHADER
	({
		attribute vec2 position;
		attribute vec2 texcoord;
		attribute vec4 colorize;
		attribute float alpha;
		uniform mat4 transform;
		varying vec2 _texcoord;
		varying vec4 _colorize;
		varying float _alpha;

		void main()
		{
			vec4 p = transform * vec4(position.x, position.y, 0, 1);

			_texcoord = texcoord;
			_colorize = colorize;
			_alpha = alpha;

			gl_Position = vec4(p.x, p.y, 0.5, 1.0);
		}
	}),
	FRAGMENT_SHADER
	({
		uniform sampler2D texture;
		varying vec2 _texcoord;
		varying vec4 _colorize;
		varying float _alpha;

		void main()
		{
			vec4 color = texture2D(texture, _texcoord);
			color.rgb = mix(color.rgb, _colorize.rgb * color.a, _colorize.a);
			color = color * clamp(_alpha, 0.0, 1.0);

			gl_FragColor = color;
		}
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


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
