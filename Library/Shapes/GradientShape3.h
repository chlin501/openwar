// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorLineRenderer_H
#define ColorLineRenderer_H

#include "Graphics/graphicscontext.h"
#include "Graphics/shaderprogram.h"


class GradientShape3
{
protected:
	typedef vertex2<glm::vec3, glm::vec4> vertex;
	typedef shaderprogram2<glm::vec3, glm::vec4> shaderprogram;

	shaderprogram* _shaderprogram;
	vertexbuffer<vertex> _vbo;

public:
	GradientShape3(graphicscontext* gc);
	virtual ~GradientShape3();

	virtual void Reset() = 0;
	void Draw(const glm::mat4x4& transform);
};


class GradientLineShape3 : public GradientShape3
{
public:
	GradientLineShape3(graphicscontext* gc) : GradientShape3(gc) { }
	virtual void Reset();
	void AddLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c1, const glm::vec4& c2);
};


class GradientTriangleShape3 : public GradientShape3
{
public:
	GradientTriangleShape3(graphicscontext* gc) : GradientShape3(gc) { }
	virtual void Reset();
	void AddVertex(const glm::vec3& p, const glm::vec4& c);
};


class GradientTriangleStripShape3 : public GradientShape3
{
public:
	GradientTriangleStripShape3(graphicscontext* gc) : GradientShape3(gc) { }
	virtual void Reset();
	void AddVertex(const glm::vec3& p, const glm::vec4& c, bool separator = false);
};


#endif
