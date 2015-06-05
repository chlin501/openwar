// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Widget_H
#define Widget_H

#include "Graphics/Vertex.h"
#include <vector>

class GraphicsContext;
class ScrollerViewport;
class Touch;
class Viewport2D;
class WidgetOwner;
class WidgetView;


class Widget
{
	friend class WidgetOwner;

	WidgetOwner* _widgetOwner;
	bool _visible{true};

public:
	Widget(WidgetOwner* widgetOwner);
	virtual ~Widget();

	Widget(const Widget&) = delete;
	Widget& operator=(const Widget&) = delete;

	Widget(Widget&&);
	Widget& operator=(Widget&&) = delete;

	WidgetOwner* GetWidgetOwner() const;
	WidgetView* GetWidgetView() const;

	const Viewport2D& GetViewport2D() const;
	GraphicsContext* GetGraphicsContext() const;

	virtual bool IsVisible() const;
	virtual void SetVisible(bool value);

	virtual void OrderFront();
	virtual void OrderFrontOf(Widget* widget);
	virtual void OrderBack();
	virtual void OrderBackOf(Widget* widget);

	virtual void OnTouchEnter(Touch* touch) = 0;
	virtual void OnTouchBegin(Touch* touch) = 0;

	virtual void RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices) = 0;
};


class WidgetOwner
{
	friend class Widget;
	friend class WidgetGroup;
	std::vector<Widget*> _widgets;

public:
	WidgetOwner();
	virtual ~WidgetOwner();

	WidgetOwner(const WidgetOwner&) = delete;
	WidgetOwner& operator=(const WidgetOwner&) = delete;

	//WidgetOwner(WidgetOwner&&) = default;
	//WidgetOwner& operator=(WidgetOwner&&) = default;

protected:
	const std::vector<Widget*>& GetWidgets();

	void CallWidgets_OnTouchEnter(Touch* touch);
	void CallWidgets_OnTouchBegin(Touch* touch);

	void CallWidgets_AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

	virtual WidgetView* FindWidgetView() = 0;
};


class WidgetGroup : public Widget, public WidgetOwner
{
public:
	WidgetGroup(WidgetOwner* widgetOwner);

	//WidgetGroup(const WidgetGroup&) = delete;
	//WidgetGroup& operator=(const WidgetGroup&) = delete;

	virtual void OnTouchEnter(Touch* touch);
	virtual void OnTouchBegin(Touch* touch);

	virtual void RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

protected:
	virtual WidgetView* FindWidgetView();
};


#endif
