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
	GLsizei _count;

	VertexBufferBase();
	virtual ~VertexBufferBase();

	virtual const void* data() const = 0;
	virtual GLsizei count() const = 0;

private:
	VertexBufferBase(const VertexBufferBase&) {}
	VertexBufferBase& operator=(const VertexBufferBase&) { return *this; }
};



template <class _Vertex>
class VertexBuffer : public VertexBufferBase
{
	friend class VertexGlyph<_Vertex>;

public:
	typedef _Vertex VertexT;

private:
	std::vector<VertexGlyph<VertexT>*> _glyphs;

public:
	std::vector<VertexT> _vertices;

	VertexBuffer()
	{
	}

	void Clear()
	{
		_vertices.clear();
	}

	virtual const void* data() const
	{
		return _vertices.data();
	}

	virtual GLsizei count() const
	{
		return _vbo != 0 ? _count : (GLsizei)_vertices.size();
	}

	void ClearGlyphs()
	{
		for (VertexGlyph<VertexT>* glyph : _glyphs)
			glyph->_vertexBuffer = nullptr;
		_glyphs.clear();
	}

	void AddGlyph(VertexGlyph<VertexT>* glyph)
	{
		if (glyph->_vertexBuffer != nullptr)
			glyph->_vertexBuffer->RemoveGlyph(glyph);
		glyph->_vertexBuffer = this;
		_glyphs.push_back(glyph);
	}

	void RemoveGlyph(VertexGlyph<VertexT>* glyph)
	{
		glyph->_vertexBuffer = nullptr;
		_glyphs.erase(
			std::find(_glyphs.begin(), _glyphs.end(), glyph),
			_glyphs.end());
	}

	void AddVertex(const VertexT& vertex)
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

		GLsizeiptr size = sizeof(VertexT) * _vertices.size();
		const GLvoid* data = _vertices.data();

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		CHECK_ERROR_GL();
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
		CHECK_ERROR_GL();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_ERROR_GL();

		_count = (GLsizei)_vertices.size();
	}


	VertexBuffer<VertexT>& UpdateVBOFromGlyphs()
	{
		_vertices.clear();
		for (VertexGlyph<VertexT>* glyph : _glyphs)
		{
			if (glyph->_rebuild)
				glyph->_rebuild(_vertices);
		}
		UpdateVBO(GL_STATIC_DRAW);
		return *this;
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
