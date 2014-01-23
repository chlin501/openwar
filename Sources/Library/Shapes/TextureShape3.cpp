// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TextureShape3.h"


TextureShape3::TextureShape3(graphicscontext* gc)
{
	static int shaderprogram_id = graphicscontext::generate_shaderprogram_id();

	_shaderprogram = gc->load_shaderprogram2<glm::vec3, glm::vec2>(
		shaderprogram_id,
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
		})
	);
	_shaderprogram->_blend_sfactor = GL_ONE;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


TextureShape3::~TextureShape3()
{
}


void TextureTriangleShape3::Reset()
{
	_vbo._mode = GL_TRIANGLES;
	_vbo._vertices.clear();
}


void TextureShape3::Draw(const glm::mat4x4& transform, const texture* texturex)
{
	_shaderprogram->get_uniform<glm::mat4>("transform").set_value(transform);
	_shaderprogram->get_uniform<const texture*>("texture").set_value(texturex);
	_shaderprogram->render(_vbo);
}


/***/


TextureTriangleShape3::~TextureTriangleShape3()
{
}


void TextureTriangleShape3::AddVertex(glm::vec3 p, glm::vec2 t)
{
	_vbo._vertices.push_back(vertex(p, t));
}
