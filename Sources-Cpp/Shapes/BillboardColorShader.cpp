// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BillboardColorShader.h"



BillboardColorShader::BillboardColorShader(GraphicsContext* gc) : ShaderProgram(
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
	}))
{
	_blend_sfactor = GL_SRC_ALPHA;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}
