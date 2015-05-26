// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef WidgetView_H
#define WidgetView_H

#include <vector>
#include "Algebra/bounds.h"
#include "Graphics/VertexBuffer.h"
#include "Surface/ScrollerHotspot.h"
#include "Graphics/Viewport.h"
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
	ScrollerViewport* _viewport2D;
	ScrollerHotspot _scrollerHotspot;
	TextureAtlas* _textureAtlas;
	WidgetVertexBuffer _vertices;

public:
	WidgetView(ViewOwner* viewOwner, std::shared_ptr<ScrollerViewport> viewport);
	virtual ~WidgetView();

	const Viewport2D& GetViewport2D() const { return *_viewport2D; }
	Viewport2D& GetViewport2D() { return *_viewport2D; }

	ScrollerViewport* GetScrollerViewport();

	TextureAtlas* GetWidgetTextureAtlas() const;

	virtual void OnTouchEnter(Touch* touch);
	virtual void OnTouchBegin(Touch* touch);

	virtual void Render();

protected:
	virtual WidgetView* FindWidgetView();

private:
	void UpdateVertexBuffer();
};


#endif
