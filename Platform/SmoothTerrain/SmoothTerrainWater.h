// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainWater_H
#define SmoothTerrainWater_H

#include "ShaderProgram.h"
#include "Image.h"
#include "Shapes/VertexShape.h"

class GraphicsContext;
class GroundMap;


class WaterInsideShader : public ShaderProgram<Vertex_2f>
{
	friend class GraphicsContext;
	/*
		attribute vec2 position;

		uniform mat4 transform;
		uniform vec4 map_bounds;
	 */
	WaterInsideShader(GraphicsContext* gc);
};


class WaterBorderShader : public ShaderProgram<Vertex_2f>
{
	friend class GraphicsContext;
	/*
		attribute vec2 position;

		uniform mat4 transform;
		uniform vec4 map_bounds;
	 */
	WaterBorderShader(GraphicsContext* gc);
};



class SmoothTerrainWater
{
	GroundMap* _groundMap;

	VertexShape_2f _waterInsideVertices;
	VertexShape_2f _waterBorderVertices;

public:
	SmoothTerrainWater(GroundMap* groundMap);
	virtual ~SmoothTerrainWater();

	void Update();
	void Render(GraphicsContext* gc, const glm::mat4x4& transform);
};


#endif
