// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ColorBillboardShape.h"


ColorBillboardShape::ColorBillboardShape(graphicscontext* gc)
{
	static int _shaderprogram_id = graphicscontext::generate_shaderprogram_id();

	_shaderprogram = gc->load_shaderprogram3<glm::vec3, glm::vec4, float>(
		_shaderprogram_id,
		"position", "color", "height",
		VERTEX_SHADER
		({
			uniform mat4 transform;
			uniform vec3 upvector;
			uniform float viewport_height;
			attribute vec3 position;
			attribute vec4 color;
			attribute float height;
			varying vec4 _color;

			void main()
			{
				float scale = 0.5 * height * viewport_height;
				vec3 position2 = position + scale * upvector;
				vec4 p = transform * vec4(position, 1);
				vec4 q = transform * vec4(position2, 1);
				float s = abs(q.y / q.w - p.y / p.w);

				float a = color.a;
				if (s < 1.0)
				{
					a = a * s;
					s = 1.0;
				}

				_color = vec4(color.rgb, a);
				gl_Position = p;
				gl_PointSize = s;
			}
		}),
		FRAGMENT_SHADER
		({
			varying vec4 _color;

			void main()
			{
				gl_FragColor = _color;
			}
		}));
	_shaderprogram->_blend_sfactor = GL_SRC_ALPHA;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


ColorBillboardShape::~ColorBillboardShape()
{
}


void ColorBillboardShape::Reset()
{
	_vbo._mode = GL_POINTS;
	_vbo._vertices.clear();
}


void ColorBillboardShape::AddBillboard(const glm::vec3& position, const glm::vec4& color, float height)
{
	_vbo._vertices.push_back(vertex(position, color, height));
}


void ColorBillboardShape::Draw(const glm::mat4x4& transform, const glm::vec3 cameraUp, float viewportHeight)
{
	float viewport_height = 0.25f * shaderprogram_base::pixels_per_point() * viewportHeight;

	_shaderprogram->get_uniform<glm::mat4>("transform").set_value(transform);
	_shaderprogram->get_uniform<glm::vec3>("upvector").set_value(cameraUp);
	_shaderprogram->get_uniform<float>("viewport_height").set_value(viewport_height);
	_shaderprogram->render(_vbo);
}
