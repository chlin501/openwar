// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TextureRenderer_H
#define TextureRenderer_H

#include "GraphicsContext.h"
#include "ShaderProgram.h"


class TextureShader : public ShaderProgram2<glm::vec3, glm::vec2>
{
public:
	TextureShader(GraphicsContext* gc);

	void SetTransform(const glm::mat4x4& value);
	void SetTexture(const texture* value);
	void Render(VertexBuffer_3f_2f* vertices);
};


class TextureTriangleShape3
{
public:
	TextureShader* _shader;
	VertexBuffer_3f_2f _vertices;

	TextureTriangleShape3(GraphicsContext* gc);
	~TextureTriangleShape3();

	void Reset();
	void AddVertex(glm::vec3 p, glm::vec2 t);
	void Draw(const glm::mat4x4& transform, const texture* texture);
};


#endif
