// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Surface.h"
#include "View.h"
#include "Touch.h"
#include "GraphicsContext.h"


Surface::Surface(GraphicsContext* gc) :
	_gc(gc),
	Size(this)
{
}


Surface::~Surface()
{
	for (View* view : _views)
		view->_surface = nullptr;
}


GraphicsContext* Surface::GetGraphicsContext() const
{
	return _gc;
}


void Surface::NotifyViewsOfTouchEnter(Touch* touch)
{
	for (View* view : _views)
		if (view->IsVisible())
			view->OnTouchEnter(touch);
}


void Surface::NotifyViewsOfTouchBegin(Touch* touch)
{
	for (View* view : _views)
		if (view->IsVisible())
			view->OnTouchBegin(touch);
}


void Surface::Render()
{
	for (auto i = _views.rbegin(); i != _views.rend(); ++i)
	{
		View* view = *i;
		if (view->IsVisible())
			view->Render();
	}
}
