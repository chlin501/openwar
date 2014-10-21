// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainWater_H
#define SmoothTerrainWater_H

#include "ShaderProgram.h"
#include "Image.h"

class GraphicsContext;
class GroundMap;


class WaterInsideShader : public ShaderProgram1<glm::vec2>
{
	friend class GraphicsContext;
	WaterInsideShader(GraphicsContext* gc);
};


class WaterBorderShader : public ShaderProgram1<glm::vec2>
{
	friend class GraphicsContext;
	WaterBorderShader(GraphicsContext* gc);
};



class SmoothTerrainWater
{
	GroundMap* _groundMap;

	VertexBuffer_2f _waterInsideVertices;
	VertexBuffer_2f _waterBorderVertices;

public:
	SmoothTerrainWater(GroundMap* groundMap);
	virtual ~SmoothTerrainWater();

	void Update();
	void Render(GraphicsContext* gc, const glm::mat4x4& transform);
};


#endif
