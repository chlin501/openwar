// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainWater_H
#define SmoothTerrainWater_H

#include "Graphics/ShaderProgram.h"
#include "Graphics/Image.h"
#include "Shapes/VertexShape.h"

class GraphicsContext;
class GroundMap;


class WaterInsideShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec2 position;

		uniform mat4 transform;
		uniform vec4 map_bounds;
	 */
	WaterInsideShader(GraphicsContext* gc);
};


class WaterBorderShader : public ShaderProgram
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
	const GroundMap* _groundMap;

	VertexShape_2f _waterInsideVertices;
	VertexShape_2f _waterBorderVertices;

public:
	SmoothTerrainWater(const GroundMap* groundMap);
	virtual ~SmoothTerrainWater();

	void Update();
	void Render(GraphicsContext* gc, const glm::mat4& transform);
};


#endif
