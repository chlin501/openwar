// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ShaderProgram_H
#define ShaderProgram_H

#include "GraphicsOpenGL.h"
#include "VertexBuffer.h"
#include "texture.h"
#include <map>

#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif


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



#define VERTEX_SHADER(source) (#source)
#define FRAGMENT_SHADER(source) (#source)



class ShaderProgramBase
{
	friend class RenderCallBase;
	static float _pixels_per_point;

public:
	std::vector<renderer_vertex_attribute> _vertex_attributes;
	GLuint _program;
	GLenum _blend_sfactor;
	GLenum _blend_dfactor;

	ShaderProgramBase(std::vector<const char*> attrs, const char* vertexshader, const char* fragmentshader);
	virtual ~ShaderProgramBase();

	static float pixels_per_point();
	static void set_pixels_per_point(float value);

	static GLuint compile_shader(GLenum type, const char* source);

	static bool link_program(GLuint program);
	static bool validate_program(GLuint program);

protected:
	ShaderProgramBase(const ShaderProgramBase&) { }
	ShaderProgramBase& operator=(const ShaderProgramBase&) { return *this; }
};



template <class _Vertex>
class ShaderProgram : public ShaderProgramBase
{
public:
	typedef _Vertex VertexT;

	ShaderProgram(std::vector<const char*> attrs, const char* vertexshader, const char* fragmentshader) :
	ShaderProgramBase(attrs, vertexshader, fragmentshader)
	{
	}

	void render(GLenum mode, VertexBuffer<VertexT>& vertices)
	{
		if (vertices.count() == 0)
			return;

		glUseProgram(_program);
		CHECK_ERROR_GL();

		vertices.bind(_vertex_attributes);

		if (_blend_sfactor != GL_ONE || _blend_dfactor != GL_ZERO)
		{
			glEnable(GL_BLEND);
			CHECK_ERROR_GL();
			glBlendFunc(_blend_sfactor, _blend_dfactor);
			CHECK_ERROR_GL();
		}

		glDrawArrays(mode, 0, vertices.count());
		CHECK_ERROR_GL();

		if (_blend_sfactor != GL_ONE || _blend_dfactor != GL_ZERO)
		{
			glDisable(GL_BLEND);
			CHECK_ERROR_GL();
			glBlendFunc(GL_ONE, GL_ZERO);
			CHECK_ERROR_GL();
		}

		vertices.unbind(_vertex_attributes);
	}

private:
	ShaderProgram(const ShaderProgram& _) : ShaderProgramBase(_) { }
	ShaderProgram& operator=(const ShaderProgram&) { return *this; }
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
class ShaderProgram1 : public ShaderProgram<Vertex1<T1>>
{
public:
	ShaderProgram1(const char* a1, const char* vertexshader, const char* fragmentshader) :
	ShaderProgram<Vertex1<T1>>(makelist(a1), vertexshader, fragmentshader)
	{
		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T1*)nullptr),
			get_vertex_attribute_type((T1*)nullptr),
			sizeof(Vertex1<T1>),
			(GLintptr)(&((Vertex1<T1>*)nullptr)->_1)));
	}
};

template <class T1, class T2>
class ShaderProgram2 : public ShaderProgram<Vertex2<T1, T2>>
{
public:
	ShaderProgram2(const char* a1, const char* a2, const char* vertexshader, const char* fragmentshader) :
	ShaderProgram<Vertex2<T1, T2>>(makelist(a1, a2), vertexshader, fragmentshader)
	{
		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T1*)nullptr),
			get_vertex_attribute_type((T1*)nullptr),
			sizeof(Vertex2<T1, T2>),
			(GLintptr)(&((Vertex2<T1, T2>*)nullptr)->_1)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T2*)nullptr),
			get_vertex_attribute_type((T2*)nullptr),
			sizeof(Vertex2<T1, T2>),
			(GLintptr)(&((Vertex2<T1, T2>*)nullptr)->_2)));
	}
};

template <class T1, class T2, class T3>
class ShaderProgram3 : public ShaderProgram<Vertex3<T1, T2, T3>>
{
public:
	ShaderProgram3(const char* a1, const char* a2, const char* a3, const char* vertexshader, const char* fragmentshader) :
	ShaderProgram<Vertex3<T1, T2, T3>>(makelist(a1, a2, a3), vertexshader, fragmentshader)
	{
		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T1*)nullptr),
			get_vertex_attribute_type((T1*)nullptr),
			sizeof(Vertex3<T1, T2, T3>),
			(GLintptr)(&((Vertex3<T1, T2, T3>*)nullptr)->_1)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T2*)nullptr),
			get_vertex_attribute_type((T2*)nullptr),
			sizeof(Vertex3<T1, T2, T3>),
			(GLintptr)(&((Vertex3<T1, T2, T3>*)nullptr)->_2)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T3*)nullptr),
			get_vertex_attribute_type((T3*)nullptr),
			sizeof(Vertex3<T1, T2, T3>),
			(GLintptr)(&((Vertex3<T1, T2, T3>*)nullptr)->_3)));
	}
};

template <class T1, class T2, class T3, class T4>
class ShaderProgram4 : public ShaderProgram<Vertex4<T1, T2, T3, T4>>
{
public:
	ShaderProgram4(const char* a1, const char* a2, const char* a3, const char* a4, const char* vertexshader, const char* fragmentshader) :
	ShaderProgram<Vertex4<T1, T2, T3, T4>>(makelist(a1, a2, a3, a4), vertexshader, fragmentshader)
	{
		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T1*)nullptr),
			get_vertex_attribute_type((T1*)nullptr),
			sizeof(Vertex4<T1, T2, T3, T4>),
			(GLintptr)(&((Vertex4<T1, T2, T3, T4>*)nullptr)->_1)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T2*)nullptr),
			get_vertex_attribute_type((T2*)nullptr),
			sizeof(Vertex4<T1, T2, T3, T4>),
			(GLintptr)(&((Vertex4<T1, T2, T3, T4>*)nullptr)->_2)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T3*)nullptr),
			get_vertex_attribute_type((T3*)nullptr),
			sizeof(Vertex4<T1, T2, T3, T4>),
			(GLintptr)(&((Vertex4<T1, T2, T3, T4>*)nullptr)->_3)));

		this->_vertex_attributes.push_back(renderer_vertex_attribute(
			get_vertex_attribute_size((T4*)nullptr),
			get_vertex_attribute_type((T4*)nullptr),
			sizeof(Vertex4<T1, T2, T3, T4>),
			(GLintptr)(&((Vertex4<T1, T2, T3, T4>*)nullptr)->_4)));
	}
};


#endif
