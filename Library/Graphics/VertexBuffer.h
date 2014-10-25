// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef VertexBuffer_H
#define VertexBuffer_H

#include "Algebra/bounds.h"
#include "GraphicsOpenGL.h"
#include "Vertex.h"


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
public:
	typedef _Vertex VertexT;

	VertexBuffer() { }

private:
	VertexBuffer(const VertexBuffer&) { }
	VertexBuffer& operator=(const VertexBuffer&) { return *this; }
};


#endif
