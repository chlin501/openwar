// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TextureRenderer.h"


TextureRenderer::TextureRenderer()
{
	_renderer = new renderer<vertex>((
		VERTEX_ATTRIBUTE(vertex, _position),
		VERTEX_ATTRIBUTE(vertex, _texcoord),
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
	));
	_renderer->_blend_sfactor = GL_ONE;
	_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


TextureRenderer::~TextureRenderer()
{
	delete _renderer;
}


void TextureTriangleRenderer::Reset()
{
	_vbo._mode = GL_TRIANGLES;
	_vbo._vertices.clear();
}


void TextureRenderer::Draw(const glm::mat4x4& transform, const texture* texturex)
{
	_renderer->get_uniform<glm::mat4>("transform").set_value(transform);
	_renderer->get_uniform<const texture*>("texture").set_value(texturex);
	_renderer->render(_vbo);
}


/***/


TextureTriangleRenderer::~TextureTriangleRenderer()
{
}


void TextureTriangleRenderer::AddVertex(glm::vec3 p, glm::vec2 t)
{
	_vbo._vertices.push_back(vertex(p, t));
}
