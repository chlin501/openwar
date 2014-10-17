// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef VertexBuffer_H
#define VertexBuffer_H

#include <functional>
#include <vector>
#include "Algebra/bounds.h"
#include "GraphicsOpenGL.h"
#include "Vertex.h"
#include "VertexGlyph.h"


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

private:
	std::vector<VertexGlyphType> _glyphs;

public:

	std::vector<VertexType> _vertices;

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

	void ClearGlyphs()
	{
		_glyphs.clear();
	}

	void AddGlyph(VertexGlyphType* vertexGlyph)
	{
		_glyphs.push_back(*vertexGlyph);
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
