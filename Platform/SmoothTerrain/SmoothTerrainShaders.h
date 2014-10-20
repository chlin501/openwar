// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainShaders_H
#define SmoothTerrainShaders_H

#include "FrameBuffer.h"
#include "RenderBuffer.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"

class GraphicsContext;


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


class TerrainInsideShader : public ShaderProgram2<glm::vec3, glm::vec3>
{
public:
	TerrainInsideShader(GraphicsContext* gc);
};


class TerrainBorderShader : public ShaderProgram2<glm::vec3, glm::vec3>
{
public:
	TerrainBorderShader(GraphicsContext* gc);
};


class TerrainSkirtShader : public ShaderProgram2<glm::vec3, float>
{
public:
	TerrainSkirtShader(GraphicsContext* gc);
};



class DepthInsideShader : public ShaderProgram2<glm::vec3, glm::vec3>
{
public:
	DepthInsideShader(GraphicsContext* gc);
};


class DepthBorderShader : public ShaderProgram2<glm::vec3, glm::vec3>
{
public:
	DepthBorderShader(GraphicsContext* gc);
};


class DepthSkirtShader : public ShaderProgram2<glm::vec3, float>
{
public:
	DepthSkirtShader(GraphicsContext* gc);
};



class SobelFilterShader : public ShaderProgram2<glm::vec2, glm::vec2>
{
public:
	SobelFilterShader(GraphicsContext* gc);
};


class GroundShadowShader : public ShaderProgram1<glm::vec2>
{
public:
	GroundShadowShader(GraphicsContext* gc);
};



struct SmoothTerrainShaders
{
	TerrainInsideShader* _terrain_inside;
	TerrainBorderShader* _terrain_border;
	TerrainSkirtShader* _terrain_skirt;
	DepthInsideShader* _depth_inside;
	DepthBorderShader* _depth_border;
	DepthSkirtShader* _depth_skirt;
	SobelFilterShader* _sobel_filter;
	GroundShadowShader* _ground_shadow;

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
