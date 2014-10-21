// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef LineRenderer_H
#define LineRenderer_H

#include "GraphicsContext.h"
#include "ShaderProgram.h"


class PlainShader : public ShaderProgram1<glm::vec3>
{
	friend class GraphicsContext;
/*
	attribute vec3 position;

	uniform mat4 transform;
	uniform float point_size;
	uniform vec4 color;
 */
	PlainShader(GraphicsContext* gc);
};


class PlainShape3
{
public:
	VertexBuffer_3f _vertices;
};


class PlainLineShape3 : public PlainShape3
{
public:
	void Reset();
	void AddLine(const glm::vec3& p1, const glm::vec3& p2);
};


class PlainTriangleShape3 : public PlainShape3
{
public:
	void Reset();
	void AddVertex(const glm::vec3& p);
};


#endif
