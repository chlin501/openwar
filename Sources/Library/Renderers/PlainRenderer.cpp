// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "PlainRenderer.h"


PlainRenderer::PlainRenderer()
{
	_renderer = new renderer1<vertex>(
		"position",
		(VERTEX_ATTRIBUTE(vertex, _1)),
		VERTEX_SHADER
		({
			attribute vec3 position;
			uniform mat4 transform;
			uniform float point_size;

			void main()
			{
				vec4 p = transform * vec4(position, 1);

				gl_Position = p;
				gl_PointSize = point_size;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform vec4 color;

			void main()
			{
				gl_FragColor = color;
			}
		})
	);
	_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


PlainRenderer::~PlainRenderer()
{
	delete _renderer;
}


void PlainRenderer::Draw(const glm::mat4x4& transform, const glm::vec4& color)
{
	glLineWidth(1);

	_renderer->get_uniform<glm::mat4>("transform").set_value(transform);
	_renderer->get_uniform<float>("point_size").set_value(1);
	_renderer->get_uniform<glm::vec4>("color").set_value(color);
	_renderer->render(_vbo);
}


/***/


void PlainLineRenderer::Reset()
{
	_vbo._mode = GL_LINES;
	_vbo._vertices.clear();
}


void PlainLineRenderer::AddLine(const glm::vec3& p1, const glm::vec3& p2)
{
	_vbo._vertices.push_back(vertex(p1));
	_vbo._vertices.push_back(vertex(p2));
}


/***/


void PlainTriangleRenderer::Reset()
{
	_vbo._mode = GL_TRIANGLES;
	_vbo._vertices.clear();
}


void PlainTriangleRenderer::AddVertex(const glm::vec3& p)
{
	_vbo._vertices.push_back(vertex(p));
}
