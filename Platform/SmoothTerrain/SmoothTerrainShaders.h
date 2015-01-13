// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainShaders_H
#define SmoothTerrainShaders_H

#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderBuffer.h"
#include "Graphics/ShaderProgram.h"
#include "Shapes/VertexShape.h"

class GraphicsContext;


struct terrain_uniforms
{
	glm::mat4 _transform;
	glm::vec3 _light_normal;
	glm::vec4 _map_bounds;
	Texture* _colormap;
	Texture* _splatmap;
};


struct sobel_uniforms
{
	glm::mat4 _transform;
	Texture* _depth;
};


class TerrainInsideShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec3 position;
		attribute vec3 normal;

		uniform mat4 transform;
		uniform vec4 map_bounds;
		uniform vec3 light_normal;
	 */
	TerrainInsideShader(GraphicsContext* gc);
};


class TerrainBorderShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec3 position;
		attribute vec3 normal;

		uniform mat4 transform;
		uniform vec4 map_bounds;
		uniform vec3 light_normal;
	 */
	TerrainBorderShader(GraphicsContext* gc);
};


class TerrainSkirtShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec3 position;
		attribute float height;

		uniform mat4 transform;
	 */
	TerrainSkirtShader(GraphicsContext* gc);
};



class DepthInsideShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec3 position;
		attribute vec3 normal;

		uniform mat4 transform;
	 */
	DepthInsideShader(GraphicsContext* gc);
};


class DepthBorderShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec3 position;
		attribute vec3 normal;

		uniform mat4 transform;
		uniform vec4 map_bounds;
	 */
	DepthBorderShader(GraphicsContext* gc);
};


class DepthSkirtShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec3 position;
		attribute float height;

		uniform mat4 transform;
	 */
	DepthSkirtShader(GraphicsContext* gc);
};



class SobelFilterShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec2 position;
		attribute vec2 texcoord;

		uniform mat4 transform;
	 */
	SobelFilterShader(GraphicsContext* gc);
};


class GroundShadowShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec2 position;

		uniform mat4 transform;
		uniform vec4 map_bounds;
	 */
	GroundShadowShader(GraphicsContext* gc);
};



struct SmoothTerrainShaders
{
	void render_terrain_inside(GraphicsContext* gc, VertexShape_3f_3f& vertices, terrain_uniforms& uniforms);
	void render_terrain_border(GraphicsContext* gc, VertexShape_3f_3f& vertices, terrain_uniforms& uniforms);
	void render_terrain_skirt(GraphicsContext* gc, VertexShape_3f_1f& vertices, const glm::mat4& transform, Texture* texture);

	void render_depth_inside(GraphicsContext* gc, VertexShape_3f_3f& vertices, terrain_uniforms& uniforms);
	void render_depth_border(GraphicsContext* gc, VertexShape_3f_3f& vertices, terrain_uniforms& uniforms);
	void render_depth_skirt(GraphicsContext* gc, VertexShape_3f_1f& vertices, const glm::mat4& transform);

	void render_sobel_filter(GraphicsContext* gc, VertexShape_2f_2f& vertices, sobel_uniforms& uniforms);

	void render_ground_shadow(GraphicsContext* gc, VertexShape_2f& vertices, terrain_uniforms& uniforms);

	static Texture* create_colormap(GraphicsContext* gc);
};


#endif
