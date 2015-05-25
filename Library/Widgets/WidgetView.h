// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef WidgetView_H
#define WidgetView_H

#include <vector>
#include "Algebra/bounds.h"
#include "Graphics/VertexBuffer.h"
#include "Scroller/ScrollerHotspot.h"
#include "Scroller/ScrollerViewport.h"
#include "Surface/View.h"
#include "Widget.h"

class GraphicsContext;
class ScrollerHotspot;
class TextureAtlas;


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
	ScrollerViewport _scrollerViewport;
	ScrollerHotspot _scrollerHotspot;
	TextureAtlas* _textureAtlas;
	WidgetVertexBuffer _vertices;

public:
	WidgetView(ViewOwner* viewOwner);
	virtual ~WidgetView();

	ScrollerViewport* GetScrollerViewport();

	TextureAtlas* GetWidgetTextureAtlas() const;

	virtual void SetBounds(const bounds2f& value);

	virtual void OnTouchEnter(Touch* touch);
	virtual void OnTouchBegin(Touch* touch);

	virtual void Render();

protected:
	virtual WidgetView* FindWidgetView();

private:
	void UpdateVertexBuffer();
};


#endif
