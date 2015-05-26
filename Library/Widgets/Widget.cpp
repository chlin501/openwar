// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Widget.h"
#include "WidgetView.h"
#include "Surface/Touch.h"
#include "Surface/Surface.h"
#include "Graphics/GraphicsContext.h"
#include "Scroller/ScrollerViewport.h"
#include <cassert>


/* Widget */


Widget::Widget(WidgetOwner* widgetOwner) :
	_widgetOwner(widgetOwner),
	_visible(true)
{
	_widgetOwner->_widgets.insert(_widgetOwner->_widgets.begin(), this);
}


Widget::~Widget()
{
	if (_widgetOwner)
	{
		_widgetOwner->_widgets.erase(
			std::remove(_widgetOwner->_widgets.begin(), _widgetOwner->_widgets.end(), this),
			_widgetOwner->_widgets.end());
	}
}


Widget::Widget(Widget&& widget)
{
	_widgetOwner = widget._widgetOwner;
	widget._widgetOwner = nullptr;
	_visible = widget._visible;

	auto i = std::find(_widgetOwner->_widgets.begin(), _widgetOwner->_widgets.end(), &widget);
	assert(i != _widgetOwner->_widgets.end());
	*i = this;
}


/*Widget& Widget::operator=(Widget&& widget)
{
	_widgetOwner = widget._widgetOwner;
	widget._widgetOwner = nullptr;
	_visible = widget._visible;

	auto i = std::find(_widgetOwner->_widgets.begin(), _widgetOwner->_widgets.end(), &widget);
	assert(i != _widgetOwner->_widgets.end());
	*i = this;
	
	return *this;
}*/


/* WidgetOwner */


WidgetOwner::WidgetOwner()
{
}


WidgetOwner::~WidgetOwner()
{
	for (Widget* widget : _widgets)
		widget->_widgetOwner = nullptr;
}


void WidgetOwner::CallWidgets_OnTouchEnter(Touch* touch)
{
	for (Widget* widget : _widgets)
		if (widget->IsVisible())
			widget->OnTouchEnter(touch);
}


void WidgetOwner::CallWidgets_OnTouchBegin(Touch* touch)
{
	for (Widget* widget : _widgets)
		if (widget->IsVisible())
			widget->OnTouchBegin(touch);
}


void WidgetOwner::CallWidgets_AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
	const std::vector<Widget*>& widgets = GetWidgets();
	for (auto i = widgets.rbegin(); i != widgets.rend(); ++i)
	{
		Widget* widget = *i;
		if (widget->IsVisible())
			widget->RenderVertices(vertices);
	}
}


WidgetOwner* Widget::GetWidgetOwner() const
{
	return _widgetOwner;
}


WidgetView* Widget::GetWidgetView() const
{
	return _widgetOwner ? _widgetOwner->FindWidgetView() : nullptr;
}


ScrollableViewport2D* Widget::GetScrollerViewport() const
{
	WidgetView* widgetView = GetWidgetView();
	return widgetView ? widgetView->GetScrollerViewport() : nullptr;
}


GraphicsContext* Widget::GetGraphicsContext() const
{
	WidgetView* widgetView = GetWidgetView();
	return widgetView ? widgetView->GetGraphicsContext() : nullptr;
}


bool Widget::IsVisible() const
{
	return _visible;
}


void Widget::SetVisible(bool value)
{
	_visible = value;
}


void Widget::OrderFront()
{
	if (_widgetOwner)
	{
		auto i = std::find(_widgetOwner->_widgets.begin(), _widgetOwner->_widgets.end(), this);
		if (i != _widgetOwner->_widgets.end())
		{
			_widgetOwner->_widgets.erase(i);
			_widgetOwner->_widgets.insert(_widgetOwner->_widgets.begin(), this);
		}
	}
}


void Widget::OrderFrontOf(Widget* widget)
{
	if (_widgetOwner && _widgetOwner == widget->_widgetOwner)
	{
		auto i = std::find(_widgetOwner->_widgets.begin(), _widgetOwner->_widgets.end(), this);
		if (i != _widgetOwner->_widgets.end())
		{
			_widgetOwner->_widgets.erase(i);
			i = std::find(_widgetOwner->_widgets.begin(), _widgetOwner->_widgets.end(), widget);
			_widgetOwner->_widgets.insert(i, this);
		}
	}
}


void Widget::OrderBack()
{
	if (_widgetOwner)
	{
		auto i = std::find(_widgetOwner->_widgets.begin(), _widgetOwner->_widgets.end(), this);
		if (i != _widgetOwner->_widgets.end())
		{
			_widgetOwner->_widgets.erase(i);
			_widgetOwner->_widgets.insert(_widgetOwner->_widgets.end(), this);
		}
	}
}


void Widget::OrderBackOf(Widget* widget)
{
	if (_widgetOwner && _widgetOwner == widget->_widgetOwner)
	{
		auto i = std::find(_widgetOwner->_widgets.begin(), _widgetOwner->_widgets.end(), this);
		if (i != _widgetOwner->_widgets.end())
		{
			_widgetOwner->_widgets.erase(i);
			i = std::find(_widgetOwner->_widgets.begin(), _widgetOwner->_widgets.end(), widget);
			_widgetOwner->_widgets.insert(i + 1, this);
		}
	}
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


void WidgetGroup::OnTouchEnter(Touch* touch)
{
	CallWidgets_OnTouchEnter(touch);
}


void WidgetGroup::OnTouchBegin(Touch* touch)
{
	CallWidgets_OnTouchBegin(touch);
}


void WidgetGroup::RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
	CallWidgets_AppendVertices(vertices);
}


WidgetView* WidgetGroup::FindWidgetView()
{
	WidgetOwner* widgetOwner = GetWidgetOwner();
	return widgetOwner ? widgetOwner->FindWidgetView() : nullptr;
}
