// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Surface_H
#define Surface_H

#include <vector>

class GraphicsContext;
class Touch;
class View;
class Viewport;


class Surface
{
	friend class View;
	GraphicsContext* _gc;
	std::vector<View*> _views;

public:
	Surface(GraphicsContext* gc);
	virtual ~Surface();

	GraphicsContext* GetGraphicsContext() const;

	virtual Viewport* GetViewport() const = 0;

	void NotifyViewsOfTouchEnter(Touch* touch);
	void NotifyViewsOfTouchBegin(Touch* touch);

	virtual void Render();
};


#endif
