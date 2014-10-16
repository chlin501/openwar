// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorBillboardRenderer_H
#define ColorBillboardRenderer_H

#include "GraphicsContextX.h"
#include "ShaderProgramX.h"


class ColorBillboardShape
{
	typedef ShaderProgram3<glm::vec3, glm::vec4, float> shaderprogram;

	shaderprogram* _shaderprogram;
	VertexBufferX<Vertex_3f_4f_1f> _vbo;

public:
	ColorBillboardShape(GraphicsContext* gc);
	~ColorBillboardShape();

	void Reset();
	void AddBillboard(const glm::vec3& position, const glm::vec4& color, float height);
	void Draw(const glm::mat4x4& transform, const glm::vec3 cameraUp, float viewportHeight);
};


#endif
