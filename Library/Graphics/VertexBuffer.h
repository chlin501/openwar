// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef VertexBuffer_H
#define VertexBuffer_H

#include <functional>
#include <vector>
#include "Algebra/bounds.h"
#include "GraphicsOpenGL.h"




template <class T1>
struct Vertex1
{
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
	typedef std::function<void(std::vector<_VertexT>&)> RebuildType;

	RebuildType _rebuild;

	VertexGlyph() : _rebuild() { }
	VertexGlyph(RebuildType rebuild) : _rebuild(rebuild) { }
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
	typedef _Vertex VertexType;
	typedef VertexGlyph<VertexType> VertexGlyphType;

	std::vector<VertexType> _vertices;
	std::vector<VertexGlyphType> _glyphs;

	VertexBuffer()
	{
	}

	void Clear()
	{
		_vertices.clear();
	}

	GLsizei count() const
	{
		return _vbo != 0 ? _count : (GLsizei)_vertices.size();
	}

	void AddVertex(const VertexType& vertex)
	{
		_vertices.push_back(vertex);
	}

	virtual void UpdateVBO(GLenum usage)
	{
		if (_vbo == 0)
		{
			glGenBuffers(1, &_vbo);
			CHECK_ERROR_GL();
			if (_vbo == 0)
				return;
		}

		GLsizeiptr size = sizeof(VertexType) * _vertices.size();
		const GLvoid* data = _vertices.data();

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		CHECK_ERROR_GL();
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
		CHECK_ERROR_GL();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_ERROR_GL();

		_count = (GLsizei)_vertices.size();
	}


	VertexBuffer<VertexType>& UpdateVBOFromGlyphs()
	{
		_vertices.clear();
		for (VertexGlyphType& glyph : _glyphs)
		{
			if (glyph._rebuild)
				glyph._rebuild(_vertices);
		}
		UpdateVBO(GL_STATIC_DRAW);
		return *this;
	}

	void bind(const std::vector<renderer_vertex_attribute>& vertex_attributes)
	{
		_bind(vertex_attributes, _vertices.data());
	}

private:
	VertexBuffer(const VertexBuffer&) { }
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
