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
	static Texture* create_colormap(GraphicsContext* gc);
};


#endif
