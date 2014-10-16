// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainShaders_H
#define SmoothTerrainShaders_H

#include "FrameBuffer.h"
#include "RenderBuffer.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"


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
	ShaderProgram<Vertex_3f_3f>* _terrain_inside;
	ShaderProgram<Vertex_3f_3f>* _terrain_border;
	ShaderProgram<Vertex_3f_1f>* _terrain_skirt;
	ShaderProgram<Vertex_3f_3f>* _depth_inside;
	ShaderProgram<Vertex_3f_3f>* _depth_border;
	ShaderProgram<Vertex_3f_1f>* _depth_skirt;
	ShaderProgram<Vertex_2f_2f>* _sobel_filter;
	ShaderProgram<Vertex_2f>* _ground_shadow;

	SmoothTerrainShaders();
	~SmoothTerrainShaders();

	void render_terrain_inside(VertexBuffer_3f_3f& vertices, const terrain_uniforms& uniforms);
	void render_terrain_border(VertexBuffer_3f_3f& vertices, const terrain_uniforms& uniforms);
	void render_terrain_skirt(VertexBuffer_3f_1f& vertices, const glm::mat4& transform, const texture* texture);

	void render_depth_inside(VertexBuffer_3f_3f& vertices, const terrain_uniforms& uniforms);
	void render_depth_border(VertexBuffer_3f_3f& vertices, const terrain_uniforms& uniforms);
	void render_depth_skirt(VertexBuffer_3f_1f& vertices, const glm::mat4& transform);

	void render_sobel_filter(VertexBuffer_2f_2f& vertices, const sobel_uniforms& uniforms);

	void render_ground_shadow(VertexBuffer_2f& vertices, const terrain_uniforms& uniforms);

	static texture* create_colormap();
};



#endif
