// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef WidgetView_H
#define WidgetView_H

#include <vector>
#include "Algebra/bounds.h"
#include "VertexBuffer.h"
#include "View.h"
#include "Widget.h"

class GraphicsContext;
class ScrollerHotspot;
class ScrollerViewport;
class StringWidget;
class TextureAtlas;
class WidgetShape;


class WidgetView : public View, public WidgetOwner
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
	std::shared_ptr<ScrollerHotspot> _scrollerHotspot;

public:
	WidgetView(Surface* surface);
	virtual ~WidgetView();

	ScrollerViewport* GetScrollerViewport() const;

	TextureAtlas* GetTextureAtlas() const;

	glm::vec2 MeasureStringWidget(StringWidget* stringWidget) const;

	virtual Viewport* GetViewport() const;

	virtual void OnTouchEnter(Touch* touch);
	virtual void OnTouchBegin(Touch* touch);

	virtual void Render();

protected:
	virtual WidgetView* FindWidgetView();

private:
	void UpdateVertexBuffer();

	WidgetView(const WidgetView&) : View(nullptr), _vertices(nullptr) { }
	WidgetView& operator=(const WidgetView&) { return *this; }
};


#endif
