// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/matrix_transform.hpp>
#include "Content.h"
#include "Surface.h"
#include "CommonShaders.h"
#include "Shapes/VertexShape.h"
#import "WidgetShape.h"
#import "WidgetViewport.h"



Content::Content(GraphicsContext* gc) :
	_gc(gc),
	_viewport(new WidgetViewport(gc)),
	_flip(false)
{
}


Content::~Content()
{
}



WidgetViewport* Content::GetViewport() const
{
	return _viewport;
}
