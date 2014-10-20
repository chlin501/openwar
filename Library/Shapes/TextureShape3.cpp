// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TextureShape3.h"



TextureShader::TextureShader(GraphicsContext* gc)
{
	static int shaderprogram_id = GraphicsContext::generate_shaderprogram_id();

	_shaderprogram = gc->LoadShaderProgram2<glm::vec3, glm::vec2>(
		shaderprogram_id,
		"position", "texcoord",
		VERTEX_SHADER
		({
			uniform
			mat4 transform;
			attribute
			vec3 position;
			attribute
			vec2 texcoord;
			varying
			vec2 _texcoord;

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
			uniform
			sampler2D texture;
			varying
			vec2 _texcoord;

			void main()
			{
				gl_FragColor = texture2D(texture, _texcoord);
			}
		})
	);
	_shaderprogram->_blend_sfactor = GL_ONE;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;

}


void TextureShader::SetTransform(const glm::mat4x4& value)
{
	_shaderprogram->get_uniform<glm::mat4>("transform").set_value(value);

}


void TextureShader::SetTexture(const texture* value)
{
	_shaderprogram->get_uniform<const texture*>("texture").set_value(value);

}


void TextureShader::Render(VertexBuffer_3f_2f* vertices)
{
	_shaderprogram->render(*vertices);
}


TextureTriangleShape3::TextureTriangleShape3(GraphicsContext* gc)
{
	_shader = new TextureShader(gc);
}


TextureTriangleShape3::~TextureTriangleShape3()
{
	delete _shader;
}


void TextureTriangleShape3::Reset()
{
	_vertices._mode = GL_TRIANGLES;
	_vertices.Clear();
}


void TextureTriangleShape3::Draw(const glm::mat4x4& transform, const texture* texturex)
{
	_shader->SetTransform(transform);
	_shader->SetTexture(texturex);
	_shader->Render(&_vertices);
}


void TextureTriangleShape3::AddVertex(glm::vec3 p, glm::vec2 t)
{
	_vertices.AddVertex(Vertex_3f_2f(p, t));
}
