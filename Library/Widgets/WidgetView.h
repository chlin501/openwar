// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef WidgetView_H
#define WidgetView_H

#include "Algebra/bounds.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/Viewport.h"
#include "Surface/ScrollerHotspot.h"
#include "Surface/View.h"
#include "Widget.h"
#include <vector>

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
	Viewport2D& _viewport2D;
	TextureAtlas* _textureAtlas;
	WidgetVertexBuffer _vertices;

public:
	WidgetView(ViewOwner* viewOwner, std::shared_ptr<Viewport2D> viewport);

	const Viewport2D& GetViewport2D() const { return _viewport2D; }
	Viewport2D& GetViewport2D() { return _viewport2D; }

	TextureAtlas* GetWidgetTextureAtlas() const;

public: // View
	void OnTouchEnter(Touch* touch) override;
	void OnTouchBegin(Touch* touch) override;

	void Render() override;

protected:
	virtual WidgetView* FindWidgetView();

private:
	void UpdateVertexBuffer();
};


class ScrollerWidgetView : public WidgetView
{
	ScrollerHotspot _scrollerHotspot;

public:
	ScrollerWidgetView(ViewOwner* viewOwner, std::shared_ptr<ScrollerViewport> viewport);

public: // View
	void OnTouchEnter(Touch* touch) override;
	void OnTouchBegin(Touch* touch) override;
};


#endif
