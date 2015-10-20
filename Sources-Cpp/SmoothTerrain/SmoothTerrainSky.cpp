// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/matrix_transform.hpp>

#include "SmoothTerrainSky.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/TextureResource.h"
#include "Shapes/VertexShape.h"


SmoothTerrainSky::SmoothTerrainSky(GraphicsContext* gc) :
_gc(gc)
{
	_textureBackgroundLinen = new TextureResource(gc, Resource("Textures/Linen128x128.png"));
}


SmoothTerrainSky::~SmoothTerrainSky()
{
	delete _textureBackgroundLinen;
}


void SmoothTerrainSky::Render(Viewport* viewport, float cameraDirectionZ)
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

	vertices.AddVertex(Vertex_2f_4f(glm::vec2(-1, -0.6f), c1));
	vertices.AddVertex(Vertex_2f_4f(glm::vec2(-1,  1),    c2));
	vertices.AddVertex(Vertex_2f_4f(glm::vec2( 1,  1),    c2));
	vertices.AddVertex(Vertex_2f_4f(glm::vec2( 1,  1),    c2));
	vertices.AddVertex(Vertex_2f_4f(glm::vec2( 1, -0.6f), c1));
	vertices.AddVertex(Vertex_2f_4f(glm::vec2(-1, -0.6f), c1));

	RenderCall<GradientShader_2f>(_gc)
		.SetVertices(&vertices, "position", "color")
		.SetUniform("transform", glm::mat4())
		.SetUniform("point_size", 1.0f)
		.Render(*viewport);
}



void SmoothTerrainSky::RenderBackgroundLinen(Viewport* viewport, const bounds2f& bounds)
{
	VertexShape_2f_2f vertices;

	vertices._mode = GL_TRIANGLES;
	vertices.Clear();

	glm::vec2 size = bounds.size();
	glm::vec2 vt0 = glm::vec2();
	glm::vec2 vt1 = size / 128.0f;

	vertices.AddVertex(Vertex_2f_2f(glm::vec2(-1, -1), glm::vec2(vt0.x, vt0.y)));
	vertices.AddVertex(Vertex_2f_2f(glm::vec2(-1,  1), glm::vec2(vt0.x, vt1.y)));
	vertices.AddVertex(Vertex_2f_2f(glm::vec2( 1,  1), glm::vec2(vt1.x, vt1.y)));
	vertices.AddVertex(Vertex_2f_2f(glm::vec2( 1,  1), glm::vec2(vt1.x, vt1.y)));
	vertices.AddVertex(Vertex_2f_2f(glm::vec2( 1, -1), glm::vec2(vt1.x, vt0.y)));
	vertices.AddVertex(Vertex_2f_2f(glm::vec2(-1, -1), glm::vec2(vt0.x, vt0.y)));

	RenderCall<TextureShader_2f>(_gc)
		.SetVertices(&vertices, "position", "texcoord")
		.SetUniform("transform", glm::mat4())
		.SetTexture("texture", _textureBackgroundLinen)
		.Render(*viewport);
}
