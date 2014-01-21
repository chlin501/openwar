// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorLineRenderer_H
#define ColorLineRenderer_H

#include "../Graphics/renderer.h"


class GradientRenderer
{
protected:
	typedef vertex2<glm::vec3, glm::vec4> vertex;

	struct uniforms
	{
		glm::mat4x4 _transform;
		GLfloat _point_size;

		uniforms() : _point_size(1) {}
	};

	renderer2<glm::vec3, glm::vec4>* _renderer;
	vertexbuffer<vertex> _vbo;

public:
	GradientRenderer();
	virtual ~GradientRenderer();

	virtual void Reset() = 0;
	void Draw(const glm::mat4x4& transform);
};


class GradientLineRenderer : public GradientRenderer
{
public:
	virtual void Reset();
	void AddLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c1, const glm::vec4& c2);
};


class GradientTriangleRenderer : public GradientRenderer
{
public:
	virtual void Reset();
	void AddVertex(const glm::vec3& p, const glm::vec4& c);
};


class GradientTriangleStripRenderer : public GradientRenderer
{
public:
	virtual void Reset();
	void AddVertex(const glm::vec3& p, const glm::vec4& c, bool separator = false);
};


#endif
