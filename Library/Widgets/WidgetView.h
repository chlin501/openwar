// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef WidgetView_H
#define WidgetView_H

#include <vector>

class GraphicsContext;
class ScrollerViewport;
class WidgetShape;
class Widget;


class WidgetView
{
	GraphicsContext* _gc;
	ScrollerViewport* _viewport;
	WidgetShape* _widgetShape;

public:
	WidgetView(GraphicsContext* gc);
	~WidgetView();

	ScrollerViewport* GetViewport() const;
	WidgetShape* GetWidgetShape() const;

	virtual void Render();
};


#endif
