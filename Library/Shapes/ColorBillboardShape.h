// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorBillboardRenderer_H
#define ColorBillboardRenderer_H

#include "GraphicsContext.h"
#include "ShaderProgram.h"


class ColorBillboardShader : public ShaderProgram3<glm::vec3, glm::vec4, float>
{
public:
	ColorBillboardShader(GraphicsContext* gc);

	void SetTransform(const glm::mat4x4& value);
	void SetCameraUp(const glm::vec3 cameraUp);
	void SetViewportHeight(float viewportHeight);
	void Render(VertexBuffer_3f_4f_1f* vertices);
};


class ColorBillboardShape
{
	ColorBillboardShader* _shader;
	VertexBuffer_3f_4f_1f _vertices;

public:
	ColorBillboardShape(GraphicsContext* gc);
	~ColorBillboardShape();

	void Reset();
	void AddBillboard(const glm::vec3& position, const glm::vec4& color, float height);
	void Draw(const glm::mat4x4& transform, const glm::vec3 cameraUp, float viewportHeight);
};


#endif
