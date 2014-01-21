// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RENDERER_H
#define RENDERER_H

#include <map>
#include "vertexbuffer.h"
#include "texture.h"

#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif




enum shader_uniform_type
{
	shader_uniform_type_int,
	shader_uniform_type_float,
	shader_uniform_type_vector2,
	shader_uniform_type_vector3,
	shader_uniform_type_vector4,
	shader_uniform_type_matrix2,
	shader_uniform_type_matrix3,
	shader_uniform_type_matrix4,
	shader_uniform_type_texture
};

inline shader_uniform_type get_shader_uniform_type(int*) { return shader_uniform_type_int; }
inline shader_uniform_type get_shader_uniform_type(float*) { return shader_uniform_type_float; }
inline shader_uniform_type get_shader_uniform_type(glm::vec2*) { return shader_uniform_type_vector2; }
inline shader_uniform_type get_shader_uniform_type(glm::vec3*) { return shader_uniform_type_vector3; }
inline shader_uniform_type get_shader_uniform_type(glm::vec4*) { return shader_uniform_type_vector4; }
inline shader_uniform_type get_shader_uniform_type(glm::mat2x2*) { return shader_uniform_type_matrix2; }
inline shader_uniform_type get_shader_uniform_type(glm::mat3x3*) { return shader_uniform_type_matrix3; }
inline shader_uniform_type get_shader_uniform_type(glm::mat4x4*) { return shader_uniform_type_matrix4; }
inline shader_uniform_type get_shader_uniform_type(const texture**) { return shader_uniform_type_texture; }

inline shader_uniform_type get_shader_uniform_type(const int*) { return shader_uniform_type_int; }
inline shader_uniform_type get_shader_uniform_type(const float*) { return shader_uniform_type_float; }
inline shader_uniform_type get_shader_uniform_type(const glm::vec2*) { return shader_uniform_type_vector2; }
inline shader_uniform_type get_shader_uniform_type(const glm::vec3*) { return shader_uniform_type_vector3; }
inline shader_uniform_type get_shader_uniform_type(const glm::vec4*) { return shader_uniform_type_vector4; }
inline shader_uniform_type get_shader_uniform_type(const glm::mat2x2*) { return shader_uniform_type_matrix2; }
inline shader_uniform_type get_shader_uniform_type(const glm::mat3x3*) { return shader_uniform_type_matrix3; }
inline shader_uniform_type get_shader_uniform_type(const glm::mat4x4*) { return shader_uniform_type_matrix4; }
inline shader_uniform_type get_shader_uniform_type(const texture* const*) { return shader_uniform_type_texture; }



struct plain_uniforms
{
	glm::mat4x4 _transform;
};


struct gradient_uniforms
{
	glm::mat4x4 _transform;
	GLfloat _point_size;

	gradient_uniforms() : _point_size(1) {}
};


struct color_uniforms
{
	glm::mat4x4 _transform;
	GLfloat _point_size;
	glm::vec4 _color;

	color_uniforms() : _point_size(1) {}
};


struct texture_uniforms
{
	glm::mat4x4 _transform;
	const texture* _texture;
};

struct texture_alpha_uniforms
{
	glm::mat4x4 _transform;
	const texture* _texture;
	float _alpha;
};

struct string_uniforms
{
	glm::mat4x4 _transform;
	const texture* _texture;
	glm::vec4 _color;
};


struct ground_uniforms
{
	glm::mat4x4 _transform;
	const texture* _texture;
	glm::vec2 _obstacle1;
	glm::vec2 _obstacle2;
	glm::vec2 _obstacle3;
	glm::vec2 _obstacle4;
	glm::vec2 _obstacle5;
	glm::vec2 _obstacle6;
};




inline GLint get_vertex_attribute_size(float*) { return 1; }
inline GLenum get_vertex_attribute_type(float*) { return GL_FLOAT; }

inline GLint get_vertex_attribute_size(glm::vec2*) { return 2; }
inline GLenum get_vertex_attribute_type(glm::vec2*) { return GL_FLOAT; }

inline GLint get_vertex_attribute_size(glm::vec3*) { return 3; }
inline GLenum get_vertex_attribute_type(glm::vec3*) { return GL_FLOAT; }

inline GLint get_vertex_attribute_size(glm::vec4*) { return 4; }
inline GLenum get_vertex_attribute_type(glm::vec4*) { return GL_FLOAT; }


struct renderer_vertex_attribute
{
	GLint _size;
	GLenum _type;
	GLsizei _stride;
	GLintptr _offset;

	renderer_vertex_attribute(GLint size, GLenum type, GLsizei stride, GLintptr offset)
		: _size(size), _type(type), _stride(stride), _offset(offset)
	{
	}
};


template <class T>
class shader_uniform
{
	GLuint _program;
	GLint _location;
	GLenum _texture;

public:
	shader_uniform(GLuint program, GLint location, GLenum texture) :
	_program(program), _location(location), _texture(texture)
	{
	}

	void set_value(const T& value)
	{
		glUseProgram(_program);
		const void* v = (const char*)&value;
		switch (get_shader_uniform_type(&value))
		{
			case shader_uniform_type_int:
				glUniform1iv(_location, 1, (const GLint*)v);
				CHECK_ERROR_GL();
				break;

			case shader_uniform_type_float:
				glUniform1fv(_location, 1, (const GLfloat*)v);
				CHECK_ERROR_GL();
				break;

			case shader_uniform_type_vector2:
				glUniform2fv(_location, 1, (const GLfloat*)v);
				CHECK_ERROR_GL();
				break;

			case shader_uniform_type_vector3:
				glUniform3fv(_location, 1, (const GLfloat*)v);
				CHECK_ERROR_GL();
				break;

			case shader_uniform_type_vector4:
				glUniform4fv(_location, 1, (const GLfloat*)v);
				CHECK_ERROR_GL();
				break;

			case shader_uniform_type_matrix2:
				glUniformMatrix2fv(_location, 1, GL_FALSE, (const GLfloat*)v);
				CHECK_ERROR_GL();
				break;

			case shader_uniform_type_matrix3:
				glUniformMatrix3fv(_location, 1, GL_FALSE, (const GLfloat*)v);
				CHECK_ERROR_GL();
				break;

			case shader_uniform_type_matrix4:
				glUniformMatrix4fv(_location, 1, GL_FALSE, (const GLfloat*)v);
				CHECK_ERROR_GL();
				break;

			case shader_uniform_type_texture: {
				texture**t = (texture**)v;
				glActiveTexture(GL_TEXTURE0 + _texture);
				CHECK_ERROR_GL();
				if (*t != nullptr)
				{
					glBindTexture(GL_TEXTURE_2D, (*t)->id);
					CHECK_ERROR_GL();
				}
				glUniform1i(_location, _texture);
				CHECK_ERROR_GL();
				break;
			}
			default:
				break;
		}
	}
};


struct renderer_specification
{
	std::vector<renderer_vertex_attribute> _vertex_attributes;
	renderer_specification() { }
	renderer_specification(const renderer_vertex_attribute& x)
	{
		_vertex_attributes.push_back(x);
	}
};


renderer_specification operator,(renderer_vertex_attribute x, renderer_vertex_attribute y);
renderer_specification operator,(renderer_specification x, renderer_vertex_attribute y);


#define MEMBER_POINTER(_Vertex, _Name) &((_Vertex*)nullptr)->_Name

#define VERTEX_ATTRIBUTE_SIZE(_Vertex, _Name) get_vertex_attribute_size(MEMBER_POINTER(_Vertex, _Name))
#define VERTEX_ATTRIBUTE_TYPE(_Vertex, _Name) get_vertex_attribute_type(MEMBER_POINTER(_Vertex, _Name))
#define VERTEX_ATTRIBUTE_STRIDE(_Vertex, _Name) sizeof(_Vertex)
#define VERTEX_ATTRIBUTE_OFFSET(_Vertex, _Name) (GLintptr)MEMBER_POINTER(_Vertex, _Name)

#define VERTEX_ATTRIBUTE(_Vertex, _Name) \
	renderer_vertex_attribute( \
		VERTEX_ATTRIBUTE_SIZE(_Vertex, _Name), \
		VERTEX_ATTRIBUTE_TYPE(_Vertex, _Name), \
		VERTEX_ATTRIBUTE_STRIDE(_Vertex, _Name), \
		VERTEX_ATTRIBUTE_OFFSET(_Vertex, _Name))

#define VERTEX_SHADER(source) (#source)
#define FRAGMENT_SHADER(source) (#source)



class renderer_base
{
	std::map<GLint, GLenum> _uniformTexture;
	GLenum _nextUniformTexture;

public:
	std::vector<renderer_vertex_attribute> _vertex_attributes;
	GLuint _program;
	GLenum _blend_sfactor;
	GLenum _blend_dfactor;

	renderer_base(std::vector<const char*> attrs, const renderer_specification& specification, const char* vertexshader, const char* fragmentshader);
	virtual ~renderer_base();

	static float pixels_per_point();

	static GLuint compile_shader(GLenum type, const char* source);

	static bool link_program(GLuint program);
	static bool validate_program(GLuint program);

	template <class T>
	shader_uniform<T> get_uniform(const char* name)
	{
		GLint location = glGetUniformLocation(_program, name);
		GLenum texture = 0;
		auto i = _uniformTexture.find(location);
		if (i != _uniformTexture.end())
		{
			texture = i->second;
		}
		else
		{
			shader_uniform_type type = get_shader_uniform_type((const T*)nullptr);
			if (type == shader_uniform_type_texture)
				texture =  _nextUniformTexture++;
			_uniformTexture[location] = texture;
		}

		return shader_uniform<T>(_program, location, texture);
	}
};



template <class _Vertex>
class renderer : public renderer_base
{
public:
	typedef _Vertex vertex_type;

	renderer(std::vector<const char*> attrs, const renderer_specification& specification, const char* vertexshader, const char* fragmentshader) :
	renderer_base(attrs, specification, vertexshader, fragmentshader)
	{
	}

	void render(vertexbuffer<vertex_type>& shape)
	{
		if (shape.count() == 0)
			return;

		glUseProgram(_program);
		CHECK_ERROR_GL();

		shape.bind(_vertex_attributes);

		if (_blend_sfactor != GL_ONE || _blend_dfactor != GL_ZERO)
		{
			glEnable(GL_BLEND);
			CHECK_ERROR_GL();
			glBlendFunc(_blend_sfactor, _blend_dfactor);
			CHECK_ERROR_GL();
		}

		glDrawArrays(shape._mode, 0, shape.count());
		CHECK_ERROR_GL();

		if (_blend_sfactor != GL_ONE || _blend_dfactor != GL_ZERO)
		{
			glDisable(GL_BLEND);
			CHECK_ERROR_GL();
			glBlendFunc(GL_ONE, GL_ZERO);
			CHECK_ERROR_GL();
		}

		shape.unbind(_vertex_attributes);
	}
};

inline std::vector<const char*> makelist(const char* a1)
{
	std::vector<const char*> result;
	result.push_back(a1);
	return result;
}
inline std::vector<const char*> makelist(const char* a1, const char* a2)
{
	std::vector<const char*> result;
	result.push_back(a1);
	result.push_back(a2);
	return result;
}
inline std::vector<const char*> makelist(const char* a1, const char* a2, const char* a3)
{
	std::vector<const char*> result;
	result.push_back(a1);
	result.push_back(a2);
	result.push_back(a3);
	return result;
}
inline std::vector<const char*> makelist(const char* a1, const char* a2, const char* a3, const char* a4)
{
	std::vector<const char*> result;
	result.push_back(a1);
	result.push_back(a2);
	result.push_back(a3);
	result.push_back(a4);
	return result;
}


template <class _Vertex>
class renderer1 : public renderer<_Vertex>
{
public:
	renderer1(const char* a1, const renderer_specification& specification, const char* vertexshader, const char* fragmentshader) :
	renderer<_Vertex>(makelist(a1), specification, vertexshader, fragmentshader)
	{
	}
};

template <class _Vertex>
class renderer2 : public renderer<_Vertex>
{
public:
	renderer2(const char* a1, const char* a2, const renderer_specification& specification, const char* vertexshader, const char* fragmentshader) :
	renderer<_Vertex>(makelist(a1, a2), specification, vertexshader, fragmentshader)
	{
	}
};

template <class _Vertex>
class renderer3 : public renderer<_Vertex>
{
public:
	renderer3(const char* a1, const char* a2, const char* a3, const renderer_specification& specification, const char* vertexshader, const char* fragmentshader) :
	renderer<_Vertex>(makelist(a1, a2, a3), specification, vertexshader, fragmentshader)
	{
	}
};

template <class _Vertex>
class renderer4 : public renderer<_Vertex>
{
public:
	renderer4(const char* a1, const char* a2, const char* a3, const char* a4, const renderer_specification& specification, const char* vertexshader, const char* fragmentshader) :
	renderer<_Vertex>(makelist(a1, a2, a3, a4), specification, vertexshader, fragmentshader)
	{
	}
};


struct renderers
{
	static renderers* singleton;

	renderer2<texture_vertex>* _distance_renderer;
	renderer2<color_vertex>* _gradient_renderer;
	renderer2<color_vertex3>* _gradient_renderer3;
	renderer2<texture_vertex>* _ground_renderer;
	renderer1<plain_vertex>* _plain_renderer;
	renderer1<plain_vertex3>* _plain_renderer3;
	renderer2<texture_vertex>* _texture_renderer;
	renderer2<texture_vertex3>* _texture_renderer3;
	renderer2<texture_vertex>* _opaque_texture_renderer;
	renderer2<texture_vertex>* _alpha_texture_renderer;

	renderers();
};


#endif
