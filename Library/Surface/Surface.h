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
	Surface(GraphicsContext* gc);
	virtual ~Surface();

	GraphicsContext* GetGraphicsContext() const { return _gc; }

	// Content

	virtual Surface* GetSurface() const;
};


#endif
