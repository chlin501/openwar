// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "PlainShape3.h"



PlainShader::PlainShader(GraphicsContext* gc) : ShaderProgram1<glm::vec3>(
	"position",
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
	}))
{
	_blend_sfactor = GL_SRC_ALPHA;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


/***/


void PlainLineShape3::Reset()
{
	_vertices._mode = GL_LINES;
	_vertices.Clear();
}


void PlainLineShape3::AddLine(const glm::vec3& p1, const glm::vec3& p2)
{
	_vertices.AddVertex(Vertex_3f(p1));
	_vertices.AddVertex(Vertex_3f(p2));
}


/***/


void PlainTriangleShape3::Reset()
{
	_vertices._mode = GL_TRIANGLES;
	_vertices.Clear();
}


void PlainTriangleShape3::AddVertex(const glm::vec3& p)
{
	_vertices.AddVertex(Vertex_3f(p));
}
