// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainWater_H
#define SmoothTerrainWater_H

#include "../../Library/Graphics/renderer.h"
#include "../../Library/Algebra/image.h"

class SmoothGroundMap;


class SmoothTerrainWater
{
	struct ground_texture_uniforms
	{
		glm::mat4x4 _transform;
		glm::vec4 _map_bounds;
		const texture* _texture;
	};

	SmoothGroundMap* _smoothGroundMap;

	renderer<plain_vertex, ground_texture_uniforms>* _water_inside_renderer;
	renderer<plain_vertex, ground_texture_uniforms>* _water_border_renderer;

	vertexbuffer<plain_vertex> _shape_water_inside;
	vertexbuffer<plain_vertex> _shape_water_border;

public:
	SmoothTerrainWater(SmoothGroundMap* smoothGroundMap);
	virtual ~SmoothTerrainWater();

	void Update();
	void Render(const glm::mat4x4& transform);
};


#endif
