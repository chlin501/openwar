// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorBillboardRenderer_H
#define ColorBillboardRenderer_H

#include "graphicscontext.h"
#include "shaderprogram.h"


class ColorBillboardShape
{
	typedef vertex3<glm::vec3, glm::vec4, float> vertex;
	typedef shaderprogram3<glm::vec3, glm::vec4, float> shaderprogram;

	shaderprogram* _shaderprogram;
	vertexbuffer<vertex> _vbo;

public:
	ColorBillboardShape(graphicscontext* gc);
	~ColorBillboardShape();

	void Reset();
	void AddBillboard(const glm::vec3& position, const glm::vec4& color, float height);
	void Draw(const glm::mat4x4& transform, const glm::vec3 cameraUp, float viewportHeight);
};


#endif
