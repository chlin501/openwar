// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Surface_H
#define Surface_H

#include <vector>

#include "Layout.h"

class GraphicsContext;
class Touch;
class View;
class Viewport;


class Surface : public LayoutContext
{
	friend class View;
	GraphicsContext* _gc;
	std::vector<View*> _views;

public:
	LayoutSize Size;

	Surface(GraphicsContext* gc);
	virtual ~Surface();

	GraphicsContext* GetGraphicsContext() const;

	void NotifyViewsOfTouchEnter(Touch* touch);
	void NotifyViewsOfTouchBegin(Touch* touch);

	virtual void Render();
};


#endif
