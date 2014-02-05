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



#define VERTEX_SHADER(source) (#source)
#define FRAGMENT_SHADER(source) (#source)



class shaderprogram_base
{
	std::map<GLint, GLenum> _uniformTexture;
	GLenum _nextUniformTexture;

public:
	std::vector<renderer_vertex_attribute> _vertex_attributes;
	GLuint _program;
	GLenum _blend_sfactor;
	GLenum _blend_dfactor;

	shaderprogram_base(std::vector<const char*> attrs, const char* vertexshader, const char* fragmentshader);
	virtual ~shaderprogram_base();

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
class shaderprogram : public shaderprogram_base
{
public:
	typedef _Vertex vertex_type;

	shaderprogram(std::vector<const char*> attrs, const char* vertexshader, const char* fragmentshader) :
	shaderprogram_base(attrs, vertexshader, fragmentshader)
	{
	}

	void render(vertexbuffer<vertex_type>& shape)
	{
		render(shape._mode, shape);
	}

	void render(GLenum mode, vertexbuffer<vertex_type>& vbo)
	{
		if (vbo.count() == 0)
			return;

		glUseProgram(_program);
		CHECK_ERROR_GL();

		vbo.bind(_vertex_attributes);

		if (_blend_sfactor != GL_ONE || _blend_dfactor != GL_ZERO)
		{
			glEnable(GL_BLEND);
			CHECK_ERROR_GL();
			glBlendFunc(_blend_sfactor, _blend_dfactor);
			CHECK_ERROR_GL();
		}

		glDrawArrays(mode, 0, vbo.count());
		CHECK_ERROR_GL();

		if (_blend_sfactor != GL_ONE || _blend_dfactor != GL_ZERO)
		{
			glDisable(GL_BLEND);
			CHECK_ERROR_GL();
			glBlendFunc(GL_ONE, GL_ZERO);
			CHECK_ERROR_GL();
		}

		vbo.unbind(_vertex_attributes);
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


template <class T1>
class shaderprogram1 : public shaderprogram<vertex1<T1>>
{
public:
	shaderprogram1(const char* a1, const char* vertexshader, const char* fragmentshader) :
	shaderprogram<vertex1<T1>>(makelist(a1), vertexshader, fragmentshader)
	{
		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T1*)nullptr),
			get_vertex_attribute_type((T1*)nullptr),
			sizeof(vertex1<T1>),
			(GLintptr)(&((vertex1<T1>*)nullptr)->_1)));
	}
};

template <class T1, class T2>
class shaderprogram2 : public shaderprogram<vertex2<T1, T2>>
{
public:
	shaderprogram2(const char* a1, const char* a2, const char* vertexshader, const char* fragmentshader) :
	shaderprogram<vertex2<T1, T2>>(makelist(a1, a2), vertexshader, fragmentshader)
	{
		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T1*)nullptr),
			get_vertex_attribute_type((T1*)nullptr),
			sizeof(vertex2<T1, T2>),
			(GLintptr)(&((vertex2<T1, T2>*)nullptr)->_1)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T2*)nullptr),
			get_vertex_attribute_type((T2*)nullptr),
			sizeof(vertex2<T1, T2>),
			(GLintptr)(&((vertex2<T1, T2>*)nullptr)->_2)));
	}
};

template <class T1, class T2, class T3>
class shaderprogram3 : public shaderprogram<vertex3<T1, T2, T3>>
{
public:
	shaderprogram3(const char* a1, const char* a2, const char* a3, const char* vertexshader, const char* fragmentshader) :
	shaderprogram<vertex3<T1, T2, T3>>(makelist(a1, a2, a3), vertexshader, fragmentshader)
	{
		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T1*)nullptr),
			get_vertex_attribute_type((T1*)nullptr),
			sizeof(vertex3<T1, T2, T3>),
			(GLintptr)(&((vertex3<T1, T2, T3>*)nullptr)->_1)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T2*)nullptr),
			get_vertex_attribute_type((T2*)nullptr),
			sizeof(vertex3<T1, T2, T3>),
			(GLintptr)(&((vertex3<T1, T2, T3>*)nullptr)->_2)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T3*)nullptr),
			get_vertex_attribute_type((T3*)nullptr),
			sizeof(vertex3<T1, T2, T3>),
			(GLintptr)(&((vertex3<T1, T2, T3>*)nullptr)->_3)));
	}
};

template <class T1, class T2, class T3, class T4>
class shaderprogram4 : public shaderprogram<vertex4<T1, T2, T3, T4>>
{
public:
	shaderprogram4(const char* a1, const char* a2, const char* a3, const char* a4, const char* vertexshader, const char* fragmentshader) :
	shaderprogram<vertex4<T1, T2, T3, T4>>(makelist(a1, a2, a3, a4), vertexshader, fragmentshader)
	{
		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T1*)nullptr),
			get_vertex_attribute_type((T1*)nullptr),
			sizeof(vertex4<T1, T2, T3, T4>),
			(GLintptr)(&((vertex4<T1, T2, T3, T4>*)nullptr)->_1)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T2*)nullptr),
			get_vertex_attribute_type((T2*)nullptr),
			sizeof(vertex4<T1, T2, T3, T4>),
			(GLintptr)(&((vertex4<T1, T2, T3, T4>*)nullptr)->_2)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T3*)nullptr),
			get_vertex_attribute_type((T3*)nullptr),
			sizeof(vertex4<T1, T2, T3, T4>),
			(GLintptr)(&((vertex4<T1, T2, T3, T4>*)nullptr)->_3)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T4*)nullptr),
			get_vertex_attribute_type((T4*)nullptr),
			sizeof(vertex4<T1, T2, T3, T4>),
			(GLintptr)(&((vertex4<T1, T2, T3, T4>*)nullptr)->_4)));
	}
};


struct renderers
{
	static renderers* singleton;

	shaderprogram2<glm::vec2, glm::vec2>* _distance_renderer;
	shaderprogram2<glm::vec2, glm::vec4>* _gradient_renderer;
	shaderprogram2<glm::vec3, glm::vec4>* _gradient_renderer3;
	shaderprogram2<glm::vec2, glm::vec2>* _ground_renderer;
	shaderprogram1<glm::vec2>* _plain_renderer;
	shaderprogram1<glm::vec3>* _plain_renderer3;
	shaderprogram2<glm::vec2, glm::vec2>* _texture_renderer;
	shaderprogram2<glm::vec3, glm::vec2>* _texture_renderer3;
	shaderprogram2<glm::vec2, glm::vec2>* _opaque_texture_renderer;
	shaderprogram2<glm::vec2, glm::vec2>* _alpha_texture_renderer;

	renderers();
};


#endif
