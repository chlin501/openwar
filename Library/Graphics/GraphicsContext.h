// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef GraphicsContext_H
#define GraphicsContext_H

#include "GraphicsOpenGL.h"
#include "ShaderProgram.h"
#include <map>


class GraphicsContext
{
	float _pixeldensity;

public:
	GraphicsContext(float pixelDensity);
	~GraphicsContext();

	float GetPixelDensity() const { return _pixeldensity; }

private:
	GraphicsContext(const GraphicsContext&) { }
	GraphicsContext& operator=(const GraphicsContext&) { return *this; }
};


#endif
