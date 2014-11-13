// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SURFACE_H
#define SURFACE_H

class GraphicsContext;
class Touch;
class Viewport;


class Surface
{
public:
	Surface(GraphicsContext* gc);
	virtual ~Surface();

	virtual Viewport* GetViewport() const = 0;
	virtual void FindHotspots(Touch* touch) = 0;

	virtual void Render() = 0;
};


#endif
