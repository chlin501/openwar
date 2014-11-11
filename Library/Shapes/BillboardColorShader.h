// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorBillboardRenderer_H
#define ColorBillboardRenderer_H

#include "GraphicsContext.h"
#include "ShaderProgram.h"


class BillboardColorShader : public ShaderProgram
{
	friend class GraphicsContext;
/*
	attribute vec3 position;
	attribute vec4 color;
	attribute float height;

	uniform mat4 transform;
	uniform vec3 upvector;
	uniform float viewport_height;
 */
	BillboardColorShader(GraphicsContext* gc);
};


#endif
