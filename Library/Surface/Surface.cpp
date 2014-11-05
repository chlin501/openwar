// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Surface.h"
#include "Gesture.h"
#include <glm/gtc/matrix_transform.hpp>



Surface::Surface(GraphicsContext* gc) : Container(gc)
{
}


Surface::~Surface()
{
}


Surface* Surface::GetSurface() const
{
	return const_cast<Surface*>(this);
}
