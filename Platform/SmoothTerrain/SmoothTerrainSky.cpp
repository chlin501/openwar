// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/matrix_transform.hpp>

#include "SmoothTerrainSky.h"
#include "GraphicsContext.h"
#include "Shapes/VertexShape.h"
#include "TextureResource.h"


SmoothTerrainSky::SmoothTerrainSky(GraphicsContext* gc) :
_gc(gc)
{
	_textureBackgroundLinen = new TextureResource(gc, resource("Textures/Linen128x128.png"));
}


SmoothTerrainSky::~SmoothTerrainSky()
{
	delete _textureBackgroundLinen;
}


void SmoothTerrainSky::Render(const glm::mat4& transform, bounds2f bounds, float cameraDirectionZ, bool flip)
{
	VertexShape_2f_4f vertices;

	float y = cameraDirectionZ;
	float x = sqrtf(1 - y * y);
	float a = 1 - fabsf(atan2f(y, x) / (float)M_PI_2);
	float blend = bounds1f(0, 1).clamp(3.0f * (a - 0.3f));

	vertices._mode = GL_TRIANGLES;
	vertices.Clear();

	glm::vec4 c1 = glm::vec4(56, 165, 230, 0) / 255.0f;
	glm::vec4 c2 = glm::vec4(160, 207, 243, 255) / 255.0f;
	c2.a = blend;

	float x0 = bounds.min.x;
	float x1 = bounds.max.x;
	float y0 = bounds.y().mix(0.2f);
	float y1 = bounds.max.y;

	vertices.AddVertex(Vertex_2f_4f(glm::vec2(x0, y0), c1));
	vertices.AddVertex(Vertex_2f_4f(glm::vec2(x0, y1), c2));
	vertices.AddVertex(Vertex_2f_4f(glm::vec2(x1, y1), c2));
	vertices.AddVertex(Vertex_2f_4f(glm::vec2(x1, y1), c2));
	vertices.AddVertex(Vertex_2f_4f(glm::vec2(x1, y0), c1));
	vertices.AddVertex(Vertex_2f_4f(glm::vec2(x0, y0), c1));

	glm::mat4 t = transform;
	if (flip)
	{
		t = glm::translate(t, glm::vec3(bounds.center(), 0));
		t = glm::scale(t, glm::vec3(-1, -1, 1));
		t = glm::translate(t, glm::vec3(-bounds.center(), 0));
	}

	RenderCall<GradientShader_2f>(_gc)
		.SetVertices(&vertices)
		.SetUniform("transform", t)
		.SetUniform("point_size", 1)
		.Render();
}



void SmoothTerrainSky::RenderBackgroundLinen(const glm::mat4& transform, bounds2f bounds)
{
	VertexShape_2f_2f vertices;

	vertices._mode = GL_TRIANGLES;
	vertices.Clear();

	glm::vec2 size = bounds.size();
	glm::vec2 vt0 = glm::vec2();
	glm::vec2 vt1 = size / 128.0f;

	float x0 = bounds.min.x;
	float x1 = bounds.max.x;
	float y0 = bounds.min.y;
	float y1 = bounds.max.y;

	vertices.AddVertex(Vertex_2f_2f(glm::vec2(x0, y0), glm::vec2(vt0.x, vt0.y)));
	vertices.AddVertex(Vertex_2f_2f(glm::vec2(x0, y1), glm::vec2(vt0.x, vt1.y)));
	vertices.AddVertex(Vertex_2f_2f(glm::vec2(x1, y1), glm::vec2(vt1.x, vt1.y)));
	vertices.AddVertex(Vertex_2f_2f(glm::vec2(x1, y1), glm::vec2(vt1.x, vt1.y)));
	vertices.AddVertex(Vertex_2f_2f(glm::vec2(x1, y0), glm::vec2(vt1.x, vt0.y)));
	vertices.AddVertex(Vertex_2f_2f(glm::vec2(x0, y0), glm::vec2(vt0.x, vt0.y)));

	RenderCall<TextureShader_2f>(_gc)
		.SetVertices(&vertices)
		.SetUniform("transform", transform)
		.SetTexture("texture", _textureBackgroundLinen)
		.Render();
}
