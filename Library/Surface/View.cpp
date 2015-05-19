// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "View.h"
#include "Surface.h"
#include "Graphics/Viewport.h"
#include "Graphics/GraphicsContext.h"
#include "Touch.h"


View::View(ViewOwner* viewOwner) :
	_viewOwner(viewOwner),
	_visible(true)
{
	_viewOwner->_views.insert(_viewOwner->_views.begin(), this);
}


View::~View()
{
	if (_viewOwner)
	{
		_viewOwner->_views.erase(
			std::find(_viewOwner->_views.begin(), _viewOwner->_views.end(), this),
			_viewOwner->_views.end());
	}
}


ViewOwner* View::GetViewOwner() const
{
	return _viewOwner;
}


Surface* View::GetSurface() const
{
	return _viewOwner ? _viewOwner->FindSurface() : nullptr;
}


GraphicsContext* View::GetGraphicsContext() const
{
	Surface* surface = GetSurface();
	return surface ? surface->GetGraphicsContext() : nullptr;
}


bounds2f View::GetBounds() const
{
	return _bounds;
}


void View::SetBounds(const bounds2f& value)
{
	_bounds = value;
}


bool View::IsVisible() const
{
	return _visible;
}


void View::SetVisible(bool value)
{
	_visible = value;
}



void View::OrderFront()
{
	if (_viewOwner)
	{
		auto i = std::find(_viewOwner->_views.begin(), _viewOwner->_views.end(), this);
		if (i != _viewOwner->_views.end())
		{
			_viewOwner->_views.erase(i);
			_viewOwner->_views.insert(_viewOwner->_views.begin(), this);
		}
	}
}


void View::OrderFrontOf(View* view)
{
	if (_viewOwner && _viewOwner == view->_viewOwner)
	{
		auto i = std::find(_viewOwner->_views.begin(), _viewOwner->_views.end(), this);
		if (i != _viewOwner->_views.end())
		{
			_viewOwner->_views.erase(i);
			i = std::find(_viewOwner->_views.begin(), _viewOwner->_views.end(), view);
			_viewOwner->_views.insert(i, this);
		}
	}
}


void View::OrderBack()
{
	if (_viewOwner)
	{
		auto i = std::find(_viewOwner->_views.begin(), _viewOwner->_views.end(), this);
		if (i != _viewOwner->_views.end())
		{
			_viewOwner->_views.erase(i);
			_viewOwner->_views.insert(_viewOwner->_views.end(), this);
		}
	}
}


void View::OrderBackOf(View* view)
{
	if (_viewOwner && _viewOwner == view->_viewOwner)
	{
		auto i = std::find(_viewOwner->_views.begin(), _viewOwner->_views.end(), this);
		if (i != _viewOwner->_views.end())
		{
			_viewOwner->_views.erase(i);
			i = std::find(_viewOwner->_views.begin(), _viewOwner->_views.end(), view);
			_viewOwner->_views.insert(i + 1, this);
		}
	}
}



/* ViewOwner */


ViewOwner::ViewOwner()
{
}


ViewOwner::~ViewOwner()
{
	for (View* view : _views)
		view->_viewOwner = nullptr;
}


const std::vector<View*>& ViewOwner::GetViews()
{
	return _views;
}


void ViewOwner::NotifyViewsOfTouchEnter(Touch* touch)
{
	for (View* view : _views)
		if (view->IsVisible())
			view->OnTouchEnter(touch);
}


void ViewOwner::NotifyViewsOfTouchBegin(Touch* touch)
{
	for (View* view : _views)
		if (view->IsVisible())
			view->OnTouchBegin(touch);
}


void ViewOwner::RenderViews()
{
	for (auto i = _views.rbegin(); i != _views.rend(); ++i)
	{
		View* view = *i;
		if (view->IsVisible())
			view->Render();
	}
}


/* ViewGroup */


ViewGroup::ViewGroup(ViewOwner* viewOwner) : View(viewOwner)
{
}


void ViewGroup::OnTouchEnter(Touch* touch)
{
	NotifyViewsOfTouchEnter(touch);
}


void ViewGroup::OnTouchBegin(Touch* touch)
{
	NotifyViewsOfTouchBegin(touch);
}


void ViewGroup::Render()
{
	RenderViews();
}


Surface* ViewGroup::FindSurface()
{
	ViewOwner* viewOwner = GetViewOwner();
	return viewOwner ? viewOwner->FindSurface() : nullptr;
}
