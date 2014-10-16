// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "PlainShape3.h"


PlainShape3::PlainShape3(GraphicsContext* gc)
{
	static int shaderprogram_id = GraphicsContext::generate_shaderprogram_id();

	_shaderprogram = gc->LoadShaderProgram1<glm::vec3>(
		shaderprogram_id,
		"position",
		VERTEX_SHADER
		({
			attribute
			vec3 position;
			uniform
			mat4 transform;
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
			uniform
			vec4 color;

			void main()
			{
				gl_FragColor = color;
			}
		})
	);
	_shaderprogram->_blend_sfactor = GL_SRC_ALPHA;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


PlainShape3::~PlainShape3()
{
}


void PlainShape3::Draw(const glm::mat4x4& transform, const glm::vec4& color)
{
	glLineWidth(1);

	_shaderprogram->get_uniform<glm::mat4>("transform").set_value(transform);
	_shaderprogram->get_uniform<float>("point_size").set_value(1);
	_shaderprogram->get_uniform<glm::vec4>("color").set_value(color);
	_shaderprogram->render(_vbo);
}


/***/


void PlainLineShape3::Reset()
{
	_vbo._mode = GL_LINES;
	_vbo._vertices.clear();
}


void PlainLineShape3::AddLine(const glm::vec3& p1, const glm::vec3& p2)
{
	_vbo._vertices.push_back(Vertex_3f(p1));
	_vbo._vertices.push_back(Vertex_3f(p2));
}


/***/


void PlainTriangleShape3::Reset()
{
	_vbo._mode = GL_TRIANGLES;
	_vbo._vertices.clear();
}


void PlainTriangleShape3::AddVertex(const glm::vec3& p)
{
	_vbo._vertices.push_back(Vertex_3f(p));
}
