// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef LineRenderer_H
#define LineRenderer_H

#include "../Graphics/graphicscontext.h"
#include "../Graphics/shaderprogram.h"


class PlainShape3
{
protected:
	typedef vertex1<glm::vec3> vertex;
	typedef shaderprogram1<glm::vec3> shaderprogram;

	vertexbuffer<vertex> _vbo;
	shaderprogram* _shaderprogram;

public:
	PlainShape3(graphicscontext* gc);
	virtual ~PlainShape3();

	virtual void Reset() = 0;
	void Draw(const glm::mat4x4& transform, const glm::vec4& color);
};


class PlainLineShape3 : public PlainShape3
{
public:
	PlainLineShape3(graphicscontext* gc) : PlainShape3(gc) { }
	virtual void Reset();
	void AddLine(const glm::vec3& p1, const glm::vec3& p2);
};


class PlainTriangleShape3 : public PlainShape3
{
public:
	PlainTriangleShape3(graphicscontext* gc) : PlainShape3(gc) { }
	virtual void Reset();
	void AddVertex(const glm::vec3& p);
};


#endif
