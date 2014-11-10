// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef CONTENT_H
#define CONTENT_H

#include "WidgetViewport.h"


class Content
{
	GraphicsContext* _gc;
	WidgetViewport* _viewport;
	bool _flip;

public:
	Content(GraphicsContext* gc);
	virtual ~Content();

	GraphicsContext* GetGraphicsContext() const { return _gc; }

	WidgetViewport* GetViewport() const;

	bool GetFlip() const { return _flip; }
	void SetFlip(bool value) { _flip = value; }
};


#endif
