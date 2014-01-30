// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Surface.h"
#include "Gesture.h"
#include <glm/gtc/matrix_transform.hpp>



Surface::Surface(glm::vec2 size, float pixelDensity) :
_gc(nullptr)
{
	_gc = new graphicscontext(pixelDensity);
	SetSize(size);
}


Surface::~Surface()
{
}


Surface* Surface::GetSurface() const
{
	return const_cast<Surface*>(this);
}


bounds2f Surface::GetViewport() const
{
	return bounds2f(0, 0, GetSize());
}


void Surface::RenderSurface()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	UseViewport();
	Container::Render(GetViewportTransform() * GetContainerTransform() * GetContentTransform());
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
