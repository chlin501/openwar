#ifndef STRINGSHAPE_H
#define STRINGSHAPE_H

#include <map>
#include <glm/gtc/matrix_transform.hpp>


#include "Algebra/bounds.h"
#include "ShaderProgram.h"
#include "TextureAtlas.h"
#include "TextureFont.h"

class GraphicsContext;
class WidgetShape;
class StringWidget;
class ImageWidget;
class Widget;


class WidgetShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec2 position;
		attribute vec2 texcoord;
		attribute vec4 colorize;
		attribute float alpha;

		uniform mat4 transform;
		uniform sampler2D texture;
	 */
	WidgetShader(GraphicsContext* gc);
};


class WidgetShape
{
	class WidgetVertexBuffer : public VertexBuffer<Vertex_2f_2f_4f_1f>
	{
		WidgetShape* _widgetShape;
	public:
		WidgetVertexBuffer(WidgetShape* shape);
		virtual void Update();
	};

	TextureAtlas* _textureAtlas;
	WidgetVertexBuffer _vertices;
	std::vector<Widget*> _widgets;

public:
	explicit WidgetShape(TextureAtlas* textureAtlas);
	~WidgetShape();

	TextureAtlas* GetTextureAtlas() const;

	VertexBuffer<Vertex_2f_2f_4f_1f>* GetVertices();

	void ClearWidgets();
	void AddWidget(Widget* widget);
	void RemoveWidget(Widget* widget);

	glm::vec2 MeasureStringWidget(StringWidget* stringWidget) const;

private:
	void UpdateVertexBuffer();

	WidgetShape(const WidgetShape&) : _vertices(nullptr) { }
	WidgetShape& operator=(const WidgetShape&) { return *this; }
};


#include "Widget.h"
#include "ImageWidget.h"
#include "StringWidget.h"

#endif
