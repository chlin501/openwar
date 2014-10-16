// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef VertexBuffer_H
#define VertexBuffer_H

#include <functional>
#include <vector>
#include "Algebra/bounds.h"

#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#if TARGET_OS_IPHONE
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <OpenGL/gl.h>
#define glGenVertexArraysOES glGenVertexArraysAPPLE
#define glBindVertexArrayOES glBindVertexArrayAPPLE
#define glDeleteVertexArraysOES glDeleteVertexArraysAPPLE
#endif
#else
#if OPENWAR_USE_GLEW
#include <GL/glew.h>
#endif
#ifdef OPENWAR_USE_GLES2
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/gl.h>
#endif
#endif


#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif


template <class T1>
struct Vertex1
{
	typedef std::function<void(std::vector<Vertex1<T1>>&)> generator_type;
	T1 _1;
	Vertex1() { }
	Vertex1(const T1& __1) : _1(__1) { }
};
typedef Vertex1<float> Vertex_1f;
typedef Vertex1<glm::vec2> Vertex_2f;
typedef Vertex1<glm::vec3> Vertex_3f;
typedef Vertex1<glm::vec4> Vertex_4f;


template <class T1, class T2>
struct Vertex2
{
	typedef std::function<void(std::vector<Vertex2<T1, T2>>&)> generator_type;
	T1 _1;
	T2 _2;
	Vertex2() { }
	Vertex2(const T1& v1, const T2& v2) : _1(v1), _2(v2) { }
};
typedef Vertex2<glm::vec2, glm::vec4> Vertex_2f_4f;
typedef Vertex2<glm::vec2, glm::vec2> Vertex_2f_2f;
typedef Vertex2<glm::vec3, glm::vec2> Vertex_3f_2f;
typedef Vertex2<glm::vec3, float> Vertex_3f_1f;
typedef Vertex2<glm::vec3, glm::vec3> Vertex_3f_3f;
typedef Vertex2<glm::vec3, glm::vec4> Vertex_3f_4f;


template <class T1, class T2, class T3>
struct Vertex3
{
	typedef std::function<void(std::vector<Vertex3<T1, T2, T3>>&)> generator_type;
	T1 _1;
	T2 _2;
	T3 _3;
	Vertex3() { }
	Vertex3(const T1& v1, const T2& v2, const T3& v3) : _1(v1), _2(v2), _3(v3) { }
};
typedef Vertex3<glm::vec2, glm::vec2, float> Vertex_2f_2f_1f;
typedef Vertex3<glm::vec3, glm::vec4, float> Vertex_3f_4f_1f;
typedef Vertex3<glm::vec2, glm::vec2, glm::vec2> Vertex_2f_2f_2f;


template <class T1, class T2, class T3, class T4>
struct Vertex4
{
	typedef std::function<void(std::vector<Vertex4<T1, T2, T3, T4>>&)> generator_type;
	T1 _1;
	T2 _2;
	T3 _3;
	T4 _4;
	Vertex4() { }
	Vertex4(const T1& v1, const T2& v2, const T3& v3, const T4& v4) : _1(v1), _2(v2), _3(v3), _4(v4) { }
};
typedef Vertex4<glm::vec3, float, glm::vec2, glm::vec2> Vertex_3f_1f_2f_2f;




template <class _VertexT>
struct VertexGlyph
{
	typedef typename _VertexT::generator_type generator_type;

	generator_type generator;

	VertexGlyph() : generator() { }
	VertexGlyph(generator_type g) : generator(g) { }
};



struct renderer_vertex_attribute;

class VertexBufferBase
{
public:
	GLenum _mode;
	GLuint _vbo;
	GLuint _vao;
	GLsizei _count;

	VertexBufferBase();
	virtual ~VertexBufferBase();

	void _bind(const std::vector<renderer_vertex_attribute>& vertex_attributes, const void* data);
	void unbind(const std::vector<renderer_vertex_attribute>& vertex_attributes);

private:
	VertexBufferBase(const VertexBufferBase&) {}
	VertexBufferBase& operator=(const VertexBufferBase&) { return *this; }
};



template <class _Vertex>
class VertexBuffer : public VertexBufferBase
{
public:
	typedef _Vertex vertex_type;
	typedef VertexGlyph<vertex_type> vertexglyph_type;

	std::vector<vertex_type> _vertices;
	std::vector<vertexglyph_type> _glyphs;

	VertexBuffer()
	{
	}

	GLsizei count() const
	{
		return _vbo != 0 ? _count : (GLsizei)_vertices.size();
	}

	virtual void update(GLenum usage)
	{
		if (_vbo == 0)
		{
			glGenBuffers(1, &_vbo);
			CHECK_ERROR_GL();
			if (_vbo == 0)
				return;
		}

		GLsizeiptr size = sizeof(vertex_type) * _vertices.size();
		const GLvoid* data = _vertices.data();

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		CHECK_ERROR_GL();
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
		CHECK_ERROR_GL();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_ERROR_GL();

		_count = (GLsizei)_vertices.size();
	}


	VertexBuffer<vertex_type>& update_vbo()
	{
		_vertices.clear();
		for (vertexglyph_type& glyph : _glyphs)
		{
			if (glyph.generator)
				glyph.generator(_vertices);
		}
		update(GL_STATIC_DRAW);
		return *this;
	}

	void bind(const std::vector<renderer_vertex_attribute>& vertex_attributes)
	{
		_bind(vertex_attributes, _vertices.data());
	}

private:
	VertexBuffer(const VertexBuffer& other) { }
	VertexBuffer& operator=(const VertexBuffer&) { return *this; }
};

typedef VertexBuffer<Vertex_2f> VertexBuffer_2f;
typedef VertexBuffer<Vertex_3f> VertexBuffer_3f;

typedef VertexBuffer<Vertex_2f_2f> VertexBuffer_2f_2f;
typedef VertexBuffer<Vertex_2f_4f> VertexBuffer_2f_4f;
typedef VertexBuffer<Vertex_3f_1f> VertexBuffer_3f_1f;
typedef VertexBuffer<Vertex_3f_2f> VertexBuffer_3f_2f;
typedef VertexBuffer<Vertex_3f_3f> VertexBuffer_3f_3f;
typedef VertexBuffer<Vertex_3f_4f> VertexBuffer_3f_4f;

typedef VertexBuffer<Vertex_2f_2f_1f> VertexBuffer_2f_2f_1f;
typedef VertexBuffer<Vertex_3f_4f_1f> VertexBuffer_3f_4f_1f;
typedef VertexBuffer<Vertex_2f_2f_2f> VertexBuffer_2f_2f_2f;

typedef VertexBuffer<Vertex_3f_1f_2f_2f> VertexBuffer_3f_1f_2f_2f;


#endif
