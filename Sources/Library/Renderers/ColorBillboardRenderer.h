// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ColorBillboardRenderer_H
#define ColorBillboardRenderer_H

#include "shaderprogram.h"


class ColorBillboardRenderer
{
	typedef vertex3<glm::vec3, glm::vec4, float> vertex;

	struct uniforms
	{
		glm::mat4x4 _transform;
		glm::vec3 _upvector;
		float _viewport_height;
	};

	shaderprogram3<glm::vec3, glm::vec4, float>* _renderer;
	vertexbuffer<vertex> _vbo;

public:
	ColorBillboardRenderer();
	~ColorBillboardRenderer();

	void Reset();
	void AddBillboard(const glm::vec3& position, const glm::vec4& color, float height);
	void Draw(const glm::mat4x4& transform, const glm::vec3 cameraUp, float viewportHeight);
};


#endif
