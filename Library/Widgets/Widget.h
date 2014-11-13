// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Widget_H
#define Widget_H

#include "Vertex.h"
#include <vector>

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

	WidgetOwner* GetWidgetOwner();
	WidgetView* GetWidgetView();

	bool IsVisible() const;
	void SetVisible(bool value);

	virtual void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices) = 0;

private:
	Widget(const Widget&) { }
	Widget& operator=(const Widget&) { return *this; }
};


class WidgetOwner
{
	friend class Widget;
	std::vector<Widget*> _widgets;

public:
	virtual ~WidgetOwner();

	virtual WidgetView* GetWidgetView() = 0;

	void BringToFront(Widget* widget);

protected:
	const std::vector<Widget*>& GetWidgets();
};


class WidgetGroup : public Widget, public WidgetOwner
{
public:
	WidgetGroup(WidgetOwner* widgetOwner);

	virtual WidgetView* GetWidgetView();

	virtual void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);
};


#endif
