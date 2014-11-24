#include "View.h"
#include "Surface.h"
#include "Graphics/Viewport.h"
#include "Graphics/GraphicsContext.h"


View::View(Surface* surface) :
	_surface(surface),
	_visible(true)
{
	_surface->_views.insert(_surface->_views.begin(), this);
}


View::~View()
{
	if (_surface != nullptr)
	{
		_surface->_views.erase(
			std::find(_surface->_views.begin(), _surface->_views.end(), this),
			_surface->_views.end());
	}
}


Surface* View::GetSurface() const
{
	return _surface;
}


GraphicsContext* View::GetGraphicsContext() const
{
	return _surface != nullptr ? _surface->GetGraphicsContext() : nullptr;
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
	if (_surface != nullptr)
	{
		auto i = std::find(_surface->_views.begin(), _surface->_views.end(), this);
		if (i != _surface->_views.end())
		{
			_surface->_views.erase(i);
			_surface->_views.insert(_surface->_views.begin(), this);
		}
	}
}


void View::OrderFrontOf(View* view)
{
	if (_surface != nullptr && _surface == view->_surface)
	{
		auto i = std::find(_surface->_views.begin(), _surface->_views.end(), this);
		if (i != _surface->_views.end())
		{
			_surface->_views.erase(i);
			i = std::find(_surface->_views.begin(), _surface->_views.end(), view);
			_surface->_views.insert(i, this);
		}
	}
}


void View::OrderBack()
{
	if (_surface != nullptr)
	{
		auto i = std::find(_surface->_views.begin(), _surface->_views.end(), this);
		if (i != _surface->_views.end())
		{
			_surface->_views.erase(i);
			_surface->_views.insert(_surface->_views.end(), this);
		}
	}
}


void View::OrderBackOf(View* view)
{
	if (_surface != nullptr && _surface == view->_surface)
	{
		auto i = std::find(_surface->_views.begin(), _surface->_views.end(), this);
		if (i != _surface->_views.end())
		{
			_surface->_views.erase(i);
			i = std::find(_surface->_views.begin(), _surface->_views.end(), view);
			_surface->_views.insert(i + 1, this);
		}
	}
}
