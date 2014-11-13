// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Widget.h"
#include "WidgetView.h"


/* Widget */


Widget::Widget(WidgetOwner* widgetOwner) :
	_widgetOwner(widgetOwner),
	_visible(true)
{
	_widgetOwner->_widgets.push_back(this);
}


Widget::~Widget()
{
	if (_widgetOwner != nullptr)
	{
		_widgetOwner->_widgets.erase(
			std::remove(_widgetOwner->_widgets.begin(), _widgetOwner->_widgets.end(), this),
			_widgetOwner->_widgets.end());
	}
}


/* WidgetOwner */


WidgetOwner::~WidgetOwner()
{
	for (Widget* widget : _widgets)
		widget->_widgetOwner = nullptr;
}


void WidgetOwner::BringToFront(Widget* widget)
{
	auto i = std::find(_widgets.begin(), _widgets.end(), widget);
	if (i != _widgets.end())
	{
		_widgets.erase(i);
		_widgets.push_back(widget);
	}
}


WidgetOwner* Widget::GetWidgetOwner()
{
	return _widgetOwner;
}


WidgetView* Widget::GetWidgetView()
{
	return _widgetOwner != nullptr ? _widgetOwner->GetWidgetView() : nullptr;
}


bool Widget::IsVisible() const
{
	return _visible;
}


void Widget::SetVisible(bool value)
{
	_visible = value;
}


/* WidgetOwner */


const std::vector<Widget*>& WidgetOwner::GetWidgets()
{
	return _widgets;
}


/* WidgetGroup */


WidgetGroup::WidgetGroup(WidgetOwner* widgetOwner) : Widget(widgetOwner)
{
}


WidgetView* WidgetGroup::GetWidgetView()
{
	WidgetOwner* widgetOwner = GetWidgetOwner();
	return widgetOwner != nullptr ? widgetOwner->GetWidgetView() : nullptr;
}


void WidgetGroup::AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
	for (Widget* widget : GetWidgets())
		if (widget->IsVisible())
			widget->AppendVertices(vertices);
}
