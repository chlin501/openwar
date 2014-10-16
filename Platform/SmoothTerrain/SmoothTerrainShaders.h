// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainShaders_H
#define SmoothTerrainShaders_H

#include "FrameBufferX.h"
#include "RenderBufferX.h"
#include "VertexBufferX.h"
#include "ShaderProgramX.h"


typedef Vertex_3f_3f terrain_vertex;
typedef Vertex_3f_1f skirt_vertex;


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
	ShaderProgramX<terrain_vertex>* _terrain_inside;
	ShaderProgramX<terrain_vertex>* _terrain_border;
	ShaderProgramX<skirt_vertex>* _terrain_skirt;
	ShaderProgramX<terrain_vertex>* _depth_inside;
	ShaderProgramX<terrain_vertex>* _depth_border;
	ShaderProgramX<skirt_vertex>* _depth_skirt;
	ShaderProgramX<Vertex_2f_2f>* _sobel_filter;
	ShaderProgramX<Vertex_2f>* _ground_shadow;

	SmoothTerrainShaders();
	~SmoothTerrainShaders();

	void render_terrain_inside(VertexBufferX<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_terrain_border(VertexBufferX<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_terrain_skirt(VertexBufferX<skirt_vertex>& shape, const glm::mat4& transform, const texture* texture);

	void render_depth_inside(VertexBufferX<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_depth_border(VertexBufferX<terrain_vertex>& shape, const terrain_uniforms& uniforms);
	void render_depth_skirt(VertexBufferX<skirt_vertex>& shape, const glm::mat4& transform);

	void render_sobel_filter(VertexBufferX<Vertex_2f_2f>& shape, const sobel_uniforms& uniforms);

	void render_ground_shadow(VertexBufferX<Vertex_2f>& shape, const terrain_uniforms& uniforms);

	static texture* create_colormap();
};



#endif
