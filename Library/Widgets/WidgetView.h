// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef WidgetView_H
#define WidgetView_H

#include <vector>
#include "Algebra/bounds.h"
#include "VertexBuffer.h"
#include "Widget.h"

class GraphicsContext;
class ScrollerViewport;
class StringWidget;
class TextureAtlas;
class WidgetShape;


class WidgetView : public WidgetOwner
{
	class WidgetVertexBuffer : public VertexBuffer<Vertex_2f_2f_4f_1f>
	{
		WidgetView* _widgetView;
	public:
		WidgetVertexBuffer(WidgetView* widgetView);
		virtual void Update();
	};

	GraphicsContext* _gc;
	ScrollerViewport* _viewport;
	TextureAtlas* _textureAtlas;
	WidgetVertexBuffer _vertices;

public:
	WidgetView(GraphicsContext* gc);
	virtual ~WidgetView();

	ScrollerViewport* GetViewport() const;

	TextureAtlas* GetTextureAtlas() const;

	glm::vec2 MeasureStringWidget(StringWidget* stringWidget) const;

	virtual void Render();

	virtual WidgetView* GetWidgetView();

private:
	void UpdateVertexBuffer();

	WidgetView(const WidgetView&) : _vertices(nullptr) { }
	WidgetView& operator=(const WidgetView&) { return *this; }
};


#endif
