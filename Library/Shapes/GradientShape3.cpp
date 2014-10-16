// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "GradientShape3.h"


GradientShape3::GradientShape3(GraphicsContext* gc)
{
	static int shaderprogram_id = GraphicsContext::generate_shaderprogram_id();

	_shaderprogram = gc->LoadShaderProgram2<glm::vec3, glm::vec4>(
		shaderprogram_id,
		"position", "color",
		VERTEX_SHADER
		({
			attribute
			vec3 position;
			attribute
			vec4 color;
			uniform
			mat4 transform;
			uniform float point_size;
			varying
			vec4 v_color;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, position.z, 1);

				gl_Position = p;
				gl_PointSize = point_size;
				v_color = color;
			}
		}),
		FRAGMENT_SHADER
		({
			varying
			vec4 v_color;

			void main()
			{
				gl_FragColor = v_color;
			}
		})
	);
	_shaderprogram->_blend_sfactor = GL_SRC_ALPHA;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


GradientShape3::~GradientShape3()
{
}


void GradientShape3::Draw(const glm::mat4x4& transform)
{
	glLineWidth(1);

	_shaderprogram->get_uniform<glm::mat4>("transform").set_value(transform);
	_shaderprogram->get_uniform<float>("point_size").set_value(1);
	_shaderprogram->render(_vbo);
}


/***/


void GradientLineShape3::Reset()
{
	_vbo._mode = GL_LINES;
	_vbo._vertices.clear();
}


void GradientLineShape3::AddLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c1, const glm::vec4& c2)
{
	_vbo._vertices.push_back(vertex(p1, c1));
	_vbo._vertices.push_back(vertex(p2, c2));
}


/***/


void GradientTriangleShape3::Reset()
{
	_vbo._mode = GL_TRIANGLES;
	_vbo._vertices.clear();
}


void GradientTriangleShape3::AddVertex(const glm::vec3& p, const glm::vec4& c)
{
	_vbo._vertices.push_back(vertex(p, c));
}


/***/


void GradientTriangleStripShape3::Reset()
{
	_vbo._mode = GL_TRIANGLE_STRIP;
	_vbo._vertices.clear();
}


void GradientTriangleStripShape3::AddVertex(const glm::vec3& p, const glm::vec4& c, bool separator)
{
	if (separator && !_vbo._vertices.empty())
	{
		_vbo._vertices.push_back(_vbo._vertices.back());
		_vbo._vertices.push_back(vertex(p, c));
	}

	_vbo._vertices.push_back(vertex(p, c));
}
