// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef LineRenderer_H
#define LineRenderer_H

#include "GraphicsContext.h"
#include "ShaderProgram.h"


class PlainShape3
{
protected:
	typedef Vertex1<glm::vec3> vertex;
	typedef ShaderProgram1<glm::vec3> shaderprogram;

	VertexBuffer<vertex> _vbo;
	shaderprogram* _shaderprogram;

public:
	PlainShape3(GraphicsContext* gc);
	virtual ~PlainShape3();

	virtual void Reset() = 0;
	void Draw(const glm::mat4x4& transform, const glm::vec4& color);
};


class PlainLineShape3 : public PlainShape3
{
public:
	PlainLineShape3(GraphicsContext* gc) : PlainShape3(gc) { }
	virtual void Reset();
	void AddLine(const glm::vec3& p1, const glm::vec3& p2);
};


class PlainTriangleShape3 : public PlainShape3
{
public:
	PlainTriangleShape3(GraphicsContext* gc) : PlainShape3(gc) { }
	virtual void Reset();
	void AddVertex(const glm::vec3& p);
};


#endif
