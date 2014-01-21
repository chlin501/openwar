// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TextureRenderer_H
#define TextureRenderer_H

#include "shaderprogram.h"


class TextureRenderer
{
protected:
	typedef vertex2<glm::vec3, glm::vec2> vertex;

	struct uniforms
	{
		glm::mat4x4 _transform;
		const texture* _texture;
	};

	shaderprogram<vertex>* _renderer;
	vertexbuffer<vertex> _vbo;

public:
	TextureRenderer();
	virtual ~TextureRenderer();

	virtual void Reset() = 0;
	void Draw(const glm::mat4x4& transform, const texture* texture);
};


class TextureTriangleRenderer : public TextureRenderer
{
public:
	virtual ~TextureTriangleRenderer();
	virtual void Reset();

	void AddVertex(glm::vec3 p, glm::vec2 t);
};


#endif
