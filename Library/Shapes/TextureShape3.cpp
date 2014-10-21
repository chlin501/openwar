// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TextureShape3.h"



TextureShader::TextureShader(GraphicsContext* gc) : ShaderProgram2<glm::vec3, glm::vec2>(
	"position", "texcoord",
	VERTEX_SHADER
	({
		uniform mat4 transform;
		attribute vec3 position;
		attribute vec2 texcoord;
		varying vec2 _texcoord;

		void main()
		{
			vec4 p = transform * vec4(position, 1);

			_texcoord = texcoord;

			gl_Position = p;
			gl_PointSize = 1.0;
		}
	}),
	FRAGMENT_SHADER
	({
		uniform sampler2D texture;
		varying vec2 _texcoord;

		void main()
		{
			gl_FragColor = texture2D(texture, _texcoord);
		}
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}



void TextureTriangleShape3::Reset()
{
	_vertices._mode = GL_TRIANGLES;
	_vertices.Clear();
}


void TextureTriangleShape3::AddVertex(glm::vec3 p, glm::vec2 t)
{
	_vertices.AddVertex(Vertex_3f_2f(p, t));
}
