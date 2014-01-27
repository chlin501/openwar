// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SURFACE_H
#define SURFACE_H

#include <vector>

#include "../Algebra/bounds.h"
#include "graphicscontext.h"
#include "Container.h"

class Gesture;
class Touch;


class Surface : public Container
{
	graphicscontext* _gc;

public:
	std::vector<Gesture*> _gestures;

	Surface(glm::vec2 size, float pixelDensity);
	virtual ~Surface();

	graphicscontext* GetGraphicsContext() const { return _gc; }

	// Content

	virtual Surface* GetSurface() const;
	virtual bounds2f GetViewport() const;

	// Surface

	virtual bool NeedsRender() const = 0;

	virtual void RenderSurface();

	virtual bool ShowContextualMenu(glm::vec2 position);

	virtual void MouseEnter(glm::vec2 position);
	virtual void MouseHover(glm::vec2 position);
	virtual void MouseLeave(glm::vec2 position);
};


#endif
