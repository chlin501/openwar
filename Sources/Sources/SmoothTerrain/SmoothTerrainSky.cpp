// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/matrix_transform.hpp>

#include "SmoothTerrainSky.h"


SmoothTerrainSky::SmoothTerrainSky()
{
	_textureBackgroundLinen = new texture(resource("Textures/Linen128x128.png"));
}


SmoothTerrainSky::~SmoothTerrainSky()
{
	delete _textureBackgroundLinen;
}


void SmoothTerrainSky::Render(const glm::mat4& transform, renderers* _renderers, bounds2f frame, float cameraDirectionZ, bool flip)
{
	vertexbuffer<color_vertex> shape;

	float y = cameraDirectionZ;
	float x = sqrtf(1 - y * y);
	float a = 1 - fabsf(atan2f(y, x) / (float)M_PI_2);
	float blend = bounds1f(0, 1).clamp(3.0f * (a - 0.3f));

	shape._mode = GL_TRIANGLES;
	shape._vertices.clear();

	glm::vec4 c1 = glm::vec4(56, 165, 230, 0) / 255.0f;
	glm::vec4 c2 = glm::vec4(160, 207, 243, 255) / 255.0f;
	c2.a = blend;

	float x0 = frame.min.x;
	float x1 = frame.max.x;
	float y0 = frame.y().lerp(0.2f);
	float y1 = frame.max.y;

	shape._vertices.push_back(color_vertex(glm::vec2(x0, y0), c1));
	shape._vertices.push_back(color_vertex(glm::vec2(x0, y1), c2));
	shape._vertices.push_back(color_vertex(glm::vec2(x1, y1), c2));
	shape._vertices.push_back(color_vertex(glm::vec2(x1, y1), c2));
	shape._vertices.push_back(color_vertex(glm::vec2(x1, y0), c1));
	shape._vertices.push_back(color_vertex(glm::vec2(x0, y0), c1));

	glm::mat4 t = transform;
	if (flip)
		t = glm::scale(t, glm::vec3(-1.0f, -1.0f, 1.0f));

	_renderers->_gradient_renderer->get_uniform<glm::mat4>("transform").set_value(t);
	_renderers->_gradient_renderer->get_uniform<float>("pixel_size").set_value(1);
	_renderers->_gradient_renderer->render(shape);
}



void SmoothTerrainSky::RenderBackgroundLinen(const glm::mat4& transform, renderers* _renderers, bounds2f frame, bool flip)
{
	vertexbuffer<texture_vertex> shape;

	shape._mode = GL_TRIANGLES;
	shape._vertices.clear();

	glm::vec2 size = frame.size();
	glm::vec2 vt0 = glm::vec2();
	glm::vec2 vt1 = size / 128.0f;

	float x0 = frame.min.x;
	float x1 = frame.max.x;
	float y0 = frame.min.y;
	float y1 = frame.max.y;

	shape._vertices.push_back(texture_vertex(glm::vec2(x0, y0), glm::vec2(vt0.x, vt0.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(x0, y1), glm::vec2(vt0.x, vt1.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(x1, y1), glm::vec2(vt1.x, vt1.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(x1, y1), glm::vec2(vt1.x, vt1.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(x1, y0), glm::vec2(vt1.x, vt0.y)));
	shape._vertices.push_back(texture_vertex(glm::vec2(x0, y0), glm::vec2(vt0.x, vt0.y)));

	_renderers->_texture_renderer->get_uniform<glm::mat4>("transform").set_value(transform);
	_renderers->_texture_renderer->get_uniform<const texture*>("texture").set_value(_textureBackgroundLinen);
	_renderers->_texture_renderer->render(shape);
}
