// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef CONTENT_H
#define CONTENT_H

#include "WidgetViewport.h"


class Content
{
	WidgetViewport* _viewport;

public:
	Content(GraphicsContext* gc);
	virtual ~Content();

	WidgetViewport* GetViewport() const;
};


#endif
