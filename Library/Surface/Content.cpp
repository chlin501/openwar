// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/matrix_transform.hpp>
#include "Content.h"



Content::Content(GraphicsContext* gc) :
	_viewport(new WidgetViewport(gc))
{
}


Content::~Content()
{
}



WidgetViewport* Content::GetViewport() const
{
	return _viewport;
}
