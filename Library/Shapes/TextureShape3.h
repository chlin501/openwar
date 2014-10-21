// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TextureRenderer_H
#define TextureRenderer_H

#include "GraphicsContext.h"
#include "ShaderProgram.h"


class TextureShader : public ShaderProgram2<glm::vec3, glm::vec2>
{
	friend class GraphicsContext;
/*
	attribute vec3 position;
	attribute vec2 texcoord;
	uniform mat4 transform;
	uniform sampler2D texture;
 */
	TextureShader(GraphicsContext* gc);
};


class TextureTriangleShape3
{
public:
	VertexBuffer_3f_2f _vertices;

	void Reset();
	void AddVertex(glm::vec3 p, glm::vec2 t);
};


#endif
