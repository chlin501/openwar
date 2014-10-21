// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorLineRenderer_H
#define ColorLineRenderer_H

#include "GraphicsContext.h"
#include "ShaderProgram.h"


class GradientShader : public ShaderProgram2<glm::vec3, glm::vec4>
{
	friend class GraphicsContext;
/*
	attribute vec3 position;
	attribute vec4 color;
	uniform mat4 transform;
	uniform float point_size;
 */
	GradientShader(GraphicsContext* gc);
};


class GradientShape3
{
public:
	VertexBuffer_3f_4f _vertices;

	virtual void Reset() = 0;
};


class GradientLineShape3 : public GradientShape3
{
public:
	virtual void Reset();
	void AddLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c1, const glm::vec4& c2);
};


class GradientTriangleShape3 : public GradientShape3
{
public:
	virtual void Reset();
	void AddVertex(const glm::vec3& p, const glm::vec4& c);
};


class GradientTriangleStripShape3 : public GradientShape3
{
public:
	virtual void Reset();
	void AddVertex(const glm::vec3& p, const glm::vec4& c, bool separator = false);
};


#endif
