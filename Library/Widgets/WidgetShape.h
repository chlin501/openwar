// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef WidgetShape_H
#define WidgetShape_H

#include "Algebra/bounds.h"
#include "VertexBuffer.h"

class StringWidget;
class TextureAtlas;
class Widget;


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


#endif
