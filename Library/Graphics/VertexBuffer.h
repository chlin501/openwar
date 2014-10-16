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
	T1 _1;

	Vertex1() { }
	Vertex1(const T1& __1) : _1(__1) { }
};

template <class T1, class T2>
struct Vertex2
{
	T1 _1;
	T2 _2;

	Vertex2() { }
	Vertex2(const T1& v1, const T2& v2) : _1(v1), _2(v2) { }
};

template <class T1, class T2, class T3>
struct Vertex3
{
	T1 _1;
	T2 _2;
	T3 _3;

	Vertex3() { }
	Vertex3(const T1& v1, const T2& v2, const T3& v3) : _1(v1), _2(v2), _3(v3) { }
};

template <class T1, class T2, class T3, class T4>
struct Vertex4
{
	T1 _1;
	T2 _2;
	T3 _3;
	T4 _4;

	Vertex4() { }
	Vertex4(const T1& v1, const T2& v2, const T3& v3, const T4& v4) : _1(v1), _2(v2), _3(v3), _4(v4) { }
};


typedef Vertex1<glm::vec2> Vertex_2f;
typedef Vertex1<glm::vec3> Vertex_3f;

typedef Vertex2<glm::vec2, glm::vec4> Vertex_2f_4f;
typedef Vertex2<glm::vec2, glm::vec2> Vertex_2f_2f;
typedef Vertex2<glm::vec3, glm::vec2> Vertex_3f_2f;
typedef Vertex2<glm::vec3, glm::vec3> Vertex_3f_3f;
typedef Vertex2<glm::vec3, float> Vertex_3f_1f;

typedef Vertex3<glm::vec2, glm::vec2, float> Vertex_2f_2f_1f;
typedef Vertex3<glm::vec3, glm::vec4, float> Vertex_3f_4f_1f;
typedef Vertex3<glm::vec2, glm::vec2, glm::vec2> Vertex_2f_2f_2f;

typedef Vertex4<glm::vec3, float, glm::vec2, glm::vec2> Vertex_3f_1f_2f_2f;



struct renderer_vertex_attribute;


class vertexbuffer_base
{
public:
	GLenum _mode;
	GLuint _vbo;
	GLuint _vao;
	GLsizei _count;

	vertexbuffer_base();
	virtual ~vertexbuffer_base();


	void _bind(const std::vector<renderer_vertex_attribute>& vertex_attributes, const void* data);
	void unbind(const std::vector<renderer_vertex_attribute>& vertex_attributes);

private:
	vertexbuffer_base(const vertexbuffer_base&) {}
	vertexbuffer_base& operator=(const vertexbuffer_base&) { return *this; }
};



template <class _Vertex>
class VertexBuffer : public vertexbuffer_base
{
public:
	typedef _Vertex vertex_type;

	std::vector<vertex_type> _vertices;

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

	void bind(const std::vector<renderer_vertex_attribute>& vertex_attributes)
	{
		_bind(vertex_attributes, _vertices.data());
	}


private:
	VertexBuffer(const VertexBuffer& other) { }
	VertexBuffer& operator=(const VertexBuffer&) { return *this; }
};

typedef VertexBuffer<Vertex_2f> VertexBuffer_2f;
typedef VertexBuffer<Vertex_2f_2f> VertexBuffer_2f_2f;



class glyphversion
{
	static int _highest_version;

public:
	int value;

	glyphversion() : value(++_highest_version) { }
	void increase() { value = ++_highest_version; }
};


template <class T1>
struct vertexglyph1
{
	typedef std::function<void(std::vector<Vertex1<T1>>& vertices)> generator_type;

	generator_type generator;
	glyphversion* version;

	vertexglyph1() : generator(), version(nullptr) { }
	vertexglyph1(generator_type g) : generator(g), version(nullptr) { }
	vertexglyph1(generator_type g, glyphversion* v) : generator(g), version(v) { }
};


template <class T1, class T2>
struct vertexglyph2
{
	typedef std::function<void(std::vector<Vertex2<T1, T2>>& vertices)> generator_type;

	generator_type generator;
	glyphversion* version;

	vertexglyph2() : generator(), version(nullptr) { }
	vertexglyph2(generator_type g) : generator(g), version(nullptr) { }
	vertexglyph2(generator_type g, glyphversion* v) : generator(g), version(v) { }
};


template <class T1, class T2, class T3>
struct vertexglyph3
{
	typedef std::function<void(std::vector<Vertex3<T1, T2, T3>>& vertices)> generator_type;

	generator_type generator;
	glyphversion* version;

	vertexglyph3() : generator(), version(nullptr) { }
	vertexglyph3(generator_type g) : generator(g), version(nullptr) { }
	vertexglyph3(generator_type g, glyphversion* v) : generator(g), version(v) { }
};


template <class T1, class T2, class T3, class T4>
struct vertexglyph4
{
	typedef std::function<void(std::vector<Vertex4<T1, T2, T3, T4>>& vertices)> generator_type;

	generator_type generator;
	glyphversion* version;

	vertexglyph4() : generator(), version(nullptr) { }
	vertexglyph4(generator_type g) : generator(g), version(nullptr) { }
	vertexglyph4(generator_type g, glyphversion* v) : generator(g), version(v) { }
};


template <class T1>
class vertexshape1
{
	typedef Vertex1<T1> vertex_type;
	typedef vertexglyph1<T1> vertexglyph_type;

	VertexBuffer<vertex_type> _vbo;
	int _version;

public:
	std::vector<vertexglyph_type> glyphs;

	vertexshape1() : _vbo(), _version(0) { }

	bool needs_update() const
	{
		/*int version = 0;
		for (const vertexglyph_type& glyph : glyphs)
		{
			if (glyph.version == nullptr)
				return true;
			version += glyph.version->value;
		}*/
		return true;//version != _version;
	}

	VertexBuffer<vertex_type>& update_vbo()
	{
		if (needs_update())
		{
			_vbo._vertices.clear();
			_version = 0;
			for (vertexglyph_type& glyph : glyphs)
			{
				if (glyph.generator)
					glyph.generator(_vbo._vertices);
				if (glyph.version != nullptr)
					_version += glyph.version->value;
			}
			_vbo.update(GL_STATIC_DRAW);
		}
		return _vbo;
	}
};


template <class T1, class T2>
class vertexshape2
{
	typedef Vertex2<T1, T2> vertex_type;
	typedef vertexglyph2<T1, T2> vertexglyph_type;

	VertexBuffer<vertex_type> _vbo;
	int _version;

public:
	std::vector<vertexglyph_type> glyphs;

	vertexshape2() : _vbo(), _version(0) { }

	bool needs_update() const
	{
		/*int version = 0;
		for (const vertexglyph_type& glyph : glyphs)
		{
			if (glyph.version == nullptr)
				return true;
			version += glyph.version->value;
		}*/
		return true;//version != _version;
	}

	VertexBuffer<vertex_type>& update_vbo()
	{
		if (needs_update())
		{
			_vbo._vertices.clear();
			_version = 0;
			for (vertexglyph_type& glyph : glyphs)
			{
				if (glyph.generator)
					glyph.generator(_vbo._vertices);
				if (glyph.version != nullptr)
					_version += glyph.version->value;
			}
			_vbo.update(GL_STATIC_DRAW);
		}
		return _vbo;
	}
};


template <class T1, class T2, class T3>
class vertexshape3
{
public:
	typedef Vertex3<T1, T2, T3> vertex_type;
	typedef vertexglyph3<T1, T2, T3> vertexglyph_type;

	VertexBuffer<vertex_type> _vbo;
	int _version;

public:
	std::vector<vertexglyph_type> glyphs;

	vertexshape3() : _vbo(), _version(0) { }

	bool needs_update() const
	{
		/*int version = 0;
		for (const vertexglyph_type& glyph : glyphs)
		{
			if (glyph.version == nullptr)
				return true;
			version += glyph.version->value;
		}*/
		return true; //version != _version;
	}

	VertexBuffer<vertex_type>& update_vbo()
	{
		if (needs_update())
		{
			_vbo._vertices.clear();
			_version = 0;
			for (vertexglyph_type& glyph : glyphs)
			{
				if (glyph.generator)
					glyph.generator(_vbo._vertices);
				if (glyph.version != nullptr)
					_version += glyph.version->value;
			}
			_vbo.update(GL_STATIC_DRAW);
		}
		return _vbo;
	}
};


template <class T1, class T2, class T3, class T4>
class vertexshape4
{
	typedef Vertex4<T1, T2, T3, T4> vertex_type;
	typedef vertexglyph4<T1, T2, T3, T4> vertexglyph_type;

	VertexBuffer<vertex_type> _vbo;
	int _version;

public:
	std::vector<vertexglyph_type> glyphs;

	vertexshape4() : _vbo(), _version(0) { }

	bool needs_update() const
	{
		/*int version = 0;
		for (const vertexglyph_type& glyph : glyphs)
		{
			if (glyph.version == nullptr)
				return true;
			version += glyph.version->value;
		}*/
		return true;//version != _version;
	}

	VertexBuffer<vertex_type>& update_vbo()
	{
		if (needs_update())
		{
			_vbo._vertices.clear();
			_version = 0;
			for (vertexglyph_type& glyph : glyphs)
			{
				if (glyph.generator)
					glyph.generator(_vbo._vertices);
				if (glyph.version != nullptr)
					_version += glyph.version->value;
			}
			_vbo.update(GL_STATIC_DRAW);
		}
		return _vbo;
	}
};



#endif
