// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef LineRenderer_H
#define LineRenderer_H

#include "GraphicsContext.h"
#include "ShaderProgram.h"


class PlainShader : public ShaderProgram1<glm::vec3>
{
public:
	PlainShader(GraphicsContext* gc);
	void SetTransform(const glm::mat4x4& value);
	void SetColor(const glm::vec4& color);
	void Render(VertexBuffer_3f* vertices);
};


class PlainShape3
{
protected:
	VertexBuffer_3f _vertices;
	PlainShader* _shader;

public:
	PlainShape3(GraphicsContext* gc);
	virtual ~PlainShape3();

	void Draw(const glm::mat4x4& transform, const glm::vec4& color);
};


class PlainLineShape3 : public PlainShape3
{
public:
	PlainLineShape3(GraphicsContext* gc) : PlainShape3(gc) { }

	void Reset();
	void AddLine(const glm::vec3& p1, const glm::vec3& p2);
};


class PlainTriangleShape3 : public PlainShape3
{
public:
	PlainTriangleShape3(GraphicsContext* gc) : PlainShape3(gc) { }

	void Reset();
	void AddVertex(const glm::vec3& p);
};


#endif
