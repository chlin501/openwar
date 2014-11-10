// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SURFACE_H
#define SURFACE_H

#include "Container.h"
#include "Renderable.h"

class GraphicsContext;


class Surface : public Container, public Renderable
{
public:
	Surface(GraphicsContext* gc);
};


#endif
