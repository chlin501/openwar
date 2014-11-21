// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Widget_H
#define Widget_H

#include "Vertex.h"
#include <vector>

class GraphicsContext;
class ScrollerViewport;
class Touch;
class WidgetOwner;
class WidgetView;


class Widget
{
	friend class WidgetOwner;
	WidgetOwner* _widgetOwner;
	bool _visible;

public:
	Widget(WidgetOwner* widgetOwner);
	virtual ~Widget();

	virtual WidgetOwner* GetWidgetOwner() const;
	virtual WidgetView* GetWidgetView() const;
	virtual ScrollerViewport* GetViewport() const;
	virtual GraphicsContext* GetGraphicsContext() const;

	virtual bool IsVisible() const;
	virtual void SetVisible(bool value);

	virtual void OrderFront();
	virtual void OrderFrontOf(Widget* widget);
	virtual void OrderBack();
	virtual void OrderBackOf(Widget* widget);

	virtual void OnTouchEnter(Touch* touch) = 0;
	virtual void OnTouchBegin(Touch* touch) = 0;

	virtual void RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices) = 0;

private:
	Widget(const Widget&) { }
	Widget& operator=(const Widget&) { return *this; }
};


class WidgetOwner
{
	friend class Widget;
	friend class WidgetGroup;
	std::vector<Widget*> _widgets;

public:
	virtual ~WidgetOwner();

protected:
	const std::vector<Widget*>& GetWidgets();

	void CallWidgets_OnTouchEnter(Touch* touch);
	void CallWidgets_OnTouchBegin(Touch* touch);

	void CallWidgets_AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

	virtual WidgetView* FindWidgetView() = 0;
};


class WidgetGroup : public Widget, protected WidgetOwner
{
public:
	WidgetGroup(WidgetOwner* widgetOwner);

	virtual void OnTouchEnter(Touch* touch);
	virtual void OnTouchBegin(Touch* touch);

	virtual void RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

protected:
	virtual WidgetView* FindWidgetView();
};


#endif
