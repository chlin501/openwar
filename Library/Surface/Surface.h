// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SURFACE_H
#define SURFACE_H

#include <vector>

#include "Algebra/bounds.h"
#include "GraphicsContext.h"
#include "Container.h"

class Gesture;
class Touch;


class Surface : public Container
{
	GraphicsContext* _gc;

public:
	std::vector<Gesture*> _gestures;

	Surface(GraphicsContext* gc, glm::ivec2 size);
	virtual ~Surface();

	GraphicsContext* GetGraphicsContext() const { return _gc; }

	// Content

	virtual Surface* GetSurface() const;
	virtual bounds2i GetViewport() const;

	virtual void OnFrameChanged();

	// Surface

	virtual bool NeedsRender() const = 0;

	virtual void RenderSurface();

	virtual bool ShowContextualMenu(glm::vec2 position);

	virtual void MouseEnter(glm::vec2 position);
	virtual void MouseHover(glm::vec2 position);
	virtual void MouseLeave(glm::vec2 position);
};


#endif
