// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RENDERER_H
#define RENDERER_H

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
	const GLchar* _name;
	GLint _size;
	GLenum _type;
	GLsizei _stride;
	GLintptr _offset;

	renderer_vertex_attribute(const GLchar* name, GLint size, GLenum type, GLsizei stride, GLintptr offset)
		: _name(name), _size(size), _type(type), _stride(stride), _offset(offset)
	{
	}
};


struct renderer_shader_uniform
{
	GLint _location;
	const GLchar* _name;
	shader_uniform_type _type;
	int _offset;
	GLenum _texture;

	renderer_shader_uniform(const GLchar* name, shader_uniform_type type, int offset)
		: _location(0), _name(name), _type(type), _offset(offset)
	{
		_texture = 0;
	}

	void set_value(const void* uniforms);
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


struct renderer_vertex_shader
{
	const GLchar* _source;

	renderer_vertex_shader(const GLchar* source) : _source(source) { }
};


struct renderer_fragment_shader
{
	const GLchar* _source;
	renderer_fragment_shader(const GLchar* source) : _source(source) { }
};


struct renderer_specification
{
	std::vector<renderer_vertex_attribute> _vertex_attributes;
	std::vector<renderer_shader_uniform> _shader_uniforms;
	const char* _vertex_shader;
	const char* _fragment_shader;
	renderer_specification() : _vertex_shader(nullptr), _fragment_shader(nullptr) { }
};


renderer_specification operator,(renderer_vertex_attribute x, renderer_vertex_attribute y);
renderer_specification operator,(renderer_vertex_attribute x, renderer_shader_uniform y);
renderer_specification operator,(renderer_vertex_attribute x, renderer_vertex_shader y);
renderer_specification operator,(renderer_specification x, renderer_vertex_attribute y);
renderer_specification operator,(renderer_specification x, renderer_shader_uniform y);
renderer_specification operator,(renderer_specification x, renderer_vertex_shader y);
renderer_specification operator,(renderer_specification x, renderer_fragment_shader y);


#define MEMBER_POINTER(_Vertex, _Name) &((_Vertex*)nullptr)->_Name

#define VERTEX_ATTRIBUTE_SIZE(_Vertex, _Name) get_vertex_attribute_size(MEMBER_POINTER(_Vertex, _Name))
#define VERTEX_ATTRIBUTE_TYPE(_Vertex, _Name) get_vertex_attribute_type(MEMBER_POINTER(_Vertex, _Name))
#define VERTEX_ATTRIBUTE_STRIDE(_Vertex, _Name) sizeof(_Vertex)
#define VERTEX_ATTRIBUTE_OFFSET(_Vertex, _Name) (GLintptr)MEMBER_POINTER(_Vertex, _Name)

#define VERTEX_ATTRIBUTE(_Vertex, _Name) \
	renderer_vertex_attribute(#_Name, \
		VERTEX_ATTRIBUTE_SIZE(_Vertex, _Name), \
		VERTEX_ATTRIBUTE_TYPE(_Vertex, _Name), \
		VERTEX_ATTRIBUTE_STRIDE(_Vertex, _Name), \
		VERTEX_ATTRIBUTE_OFFSET(_Vertex, _Name))

#define SHADER_UNIFORM_TYPE(_Uniforms, _Name) get_shader_uniform_type(MEMBER_POINTER(_Uniforms, _Name))
#define SHADER_UNIFORM_OFFSET(_Uniforms, _Name) (const char*)&((_Uniforms*)nullptr)->_Name - (const char*)nullptr


#define SHADER_UNIFORM(_Uniforms, _Name) \
	renderer_shader_uniform(#_Name, \
		SHADER_UNIFORM_TYPE(_Uniforms, _Name), \
		SHADER_UNIFORM_OFFSET(_Uniforms, _Name))


#define VERTEX_SHADER(source) renderer_vertex_shader(#source)
#define FRAGMENT_SHADER(source) renderer_fragment_shader(#source)



class renderer_base
{
public:
	std::vector<renderer_vertex_attribute> _vertex_attributes;
	std::vector<renderer_shader_uniform> _shader_uniforms;
	GLuint _program;
	GLenum _blend_sfactor;
	GLenum _blend_dfactor;

	renderer_base(const renderer_specification& specification);
	virtual ~renderer_base();

	static float pixels_per_point();

	static GLuint compile_shader(GLenum type, const char* source);

	static bool link_program(GLuint program);
	static bool validate_program(GLuint program);

	template <class T>
	shader_uniform<T> get_uniform(const char* name)
	{
		for (const renderer_shader_uniform& u : _shader_uniforms)
			if (std::strcmp(name, u._name + 1) == 0)
				return shader_uniform<T>(_program, u._location, u._texture);
		return shader_uniform<T>(0, 0, 0);
	}
};



template <class _Vertex>
class renderer : public renderer_base
{
public:
	typedef _Vertex vertex_type;

	renderer(const renderer_specification& specification) : renderer_base(specification)
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

/*
typedef renderer<plain_vertex, color_uniforms> plain_renderer;
typedef renderer<color_vertex, gradient_uniforms> gradient_renderer;
typedef renderer<texture_vertex, texture_uniforms> texture_renderer;
*/

struct renderers
{
	static renderers* singleton;

	renderer<texture_vertex>* _distance_renderer;
	renderer<color_vertex>* _gradient_renderer;
	renderer<color_vertex3>* _gradient_renderer3;
	renderer<texture_vertex>* _ground_renderer;
	renderer<plain_vertex>* _plain_renderer;
	renderer<plain_vertex3>* _plain_renderer3;
	renderer<texture_vertex>* _texture_renderer;
	renderer<texture_vertex3>* _texture_renderer3;
	renderer<texture_vertex>* _opaque_texture_renderer;
	renderer<texture_vertex>* _alpha_texture_renderer;

	renderers();
};


#endif
