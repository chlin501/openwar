// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainShaders_H
#define SmoothTerrainShaders_H

#include "../../Library/Graphics/framebuffer.h"
#include "../../Library/Graphics/renderbuffer.h"
#include "../../Library/Graphics/vertexbuffer.h"
#include "../../Library/Graphics/renderer.h"


struct terrain_vertex
{
	glm::vec3 _position;
	glm::vec3 _normal;

	terrain_vertex(glm::vec3 p, glm::vec3 n) : _position(p), _normal(n) {}
};


struct skirt_vertex
{
	glm::vec3 _position;
	float _height;

	skirt_vertex() {}
	skirt_vertex(glm::vec3 p, float h) : _position(p), _height(h) { }
};


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
	renderer<terrain_vertex, terrain_uniforms>* _terrain_inside;
	renderer<terrain_vertex, terrain_uniforms>* _terrain_border;
	renderer<skirt_vertex, texture_uniforms>* _terrain_skirt;
	renderer<terrain_vertex, terrain_uniforms>* _depth_inside;
	renderer<terrain_vertex, terrain_uniforms>* _depth_border;
	renderer<skirt_vertex, plain_uniforms>* _depth_skirt;
	renderer<texture_vertex, sobel_uniforms>* _sobel_filter;
	renderer<plain_vertex, terrain_uniforms>* _ground_shadow;

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
