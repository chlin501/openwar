// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Surface.h"

#include "shaderprogram.h"
#include "Gesture.h"



Surface::Surface(glm::vec2 size, float pixelDensity) :
_gc(nullptr)
{
	_gc = new graphicscontext(pixelDensity);
	SetSize(size);
}


Surface::~Surface()
{
}


void Surface::UseViewport()
{
	glm::vec2 size = GetSize() * _gc->get_pixeldensity();
	glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
}


void Surface::ScreenSizeChanged()
{
}


bool Surface::ShowContextualMenu(glm::vec2 position)
{
	return false;
}


void Surface::MouseEnter(glm::vec2 position)
{
}


void Surface::MouseHover(glm::vec2 position)
{
}


void Surface::MouseLeave(glm::vec2 position)
{
}
