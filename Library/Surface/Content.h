// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef CONTENT_H
#define CONTENT_H

#include <functional>
#include <memory>
#include "Algebra/bounds.h"
#include "Property.h"
#include "WidgetViewport.h"

class Container;
class GraphicsContext;
class Hotspot;
class Touch;
class WidgetShape;


class Content
{
	friend void SetContentContainer(Content*, Container*, Content*);

	GraphicsContext* _gc;
	Container* _container;
	WidgetViewport* _viewport;
	bool _visible;
	bool _flip;
	bool _dismissed;

public:
	Content(GraphicsContext* gc);
	virtual ~Content();

	virtual void Dismiss();
	virtual bool IsDismissed() const;

	GraphicsContext* GetGraphicsContext() const { return _gc; }

	virtual Container* GetContainer() const;
	virtual void SetContainer(Container* value, Content* behindContent = nullptr);

	WidgetViewport* GetViewport() const;

	virtual bool IsVisible() const;
	void SetVisible(bool value);
	bool GetFlip() const { return _flip; }
	void SetFlip(bool value) { _flip = value; }

	virtual void Update(double secondsSinceLastUpdate) = 0;
	virtual void FindHotspots(Touch* touch) = 0;
};


#endif
