// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainShaders_H
#define SmoothTerrainShaders_H

#include "../../Library/Graphics/framebuffer.h"
#include "../../Library/Graphics/renderbuffer.h"
#include "../../Library/Graphics/vertexbuffer.h"
#include "shaderprogram.h"


typedef vertex2<glm::vec3, glm::vec3> terrain_vertex;
typedef vertex2<glm::vec3, float> skirt_vertex;


struct terrain_uniforms
{
	glm::mat4x4 _transform;
	glm::vec3 _light_normal;
	glm::vec4 _map_bounds;
	const texture* _colormap;
	const texture* _splatmap;
};


struct sobel_uniforms
{
	glm::mat4x4 _transform;
	const texture* _depth;
};



struct SmoothTerrainShaders
{
	shaderprogram<terrain_vertex>* _terrain_inside;
	shaderprogram<terrain_vertex>* _terrain_border;
	shaderprogram<skirt_vertex>* _terrain_skirt;
	shaderprogram<terrain_vertex>* _depth_inside;
	shaderprogram<terrain_vertex>* _depth_border;
	shaderprogram<skirt_vertex>* _depth_skirt;
	shaderprogram<texture_vertex>* _sobel_filter;
	shaderprogram<plain_vertex>* _ground_shadow;

	SmoothTerrainShaders();
	~SmoothTerrainShaders();

	void render_terrain_inside(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_terrain_border(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_terrain_skirt(vertexbuffer<skirt_vertex>& shape, const glm::mat4& transform, const texture* texture);

	void render_depth_inside(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_depth_border(vertexbuffer<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_depth_skirt(vertexbuffer<skirt_vertex>& shape, const glm::mat4& transform);

	void render_sobel_filter(vertexbuffer<texture_vertex>& shape, const sobel_uniforms& uniforms);

	void render_ground_shadow(vertexbuffer<plain_vertex>& shape, const terrain_uniforms& uniforms);

	static texture* create_colormap();
};



#endif
