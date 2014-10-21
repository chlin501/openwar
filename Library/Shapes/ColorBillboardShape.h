// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorBillboardRenderer_H
#define ColorBillboardRenderer_H

#include "GraphicsContext.h"
#include "ShaderProgram.h"


class ColorBillboardShader : public ShaderProgram3<glm::vec3, glm::vec4, float>
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
	ColorBillboardShader(GraphicsContext* gc);
};


class ColorBillboardShape
{
public:
	VertexBuffer_3f_4f_1f _vertices;

	void Reset();
	void AddBillboard(const glm::vec3& position, const glm::vec4& color, float height);
};


#endif
