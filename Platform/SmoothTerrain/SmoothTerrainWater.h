// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainWater_H
#define SmoothTerrainWater_H

#include "ShaderProgram.h"
#include "Algebra/image.h"

class GroundMap;


class SmoothTerrainWater
{
	struct ground_texture_uniforms
	{
		glm::mat4x4 _transform;
		glm::vec4 _map_bounds;
		const texture* _texture;
	};

	GroundMap* _groundMap;

	ShaderProgram<Vertex_2f>* _water_inside_renderer;
	ShaderProgram<Vertex_2f>* _water_border_renderer;

	VertexBuffer_2f _waterInsideVertices;
	VertexBuffer_2f _waterBorderVertices;

public:
	SmoothTerrainWater(GroundMap* groundMap);
	virtual ~SmoothTerrainWater();

	void Update();
	void Render(const glm::mat4x4& transform);
};


#endif
