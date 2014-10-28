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

	virtual void Update() = 0;

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

	void UpdateVBO(GLenum mode, const VertexT* vertices, size_t count)
	{
		_mode = mode;

		if (VertexBufferBase::_vbo == 0)
		{
			glGenBuffers(1, &this->_vbo);
			CHECK_ERROR_GL();
			if (VertexBufferBase::_vbo == 0)
				return;
		}

		GLsizeiptr size = sizeof(VertexT) * count;
		const GLvoid* data = static_cast<const GLvoid*>(vertices);

		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase::_vbo);
		CHECK_ERROR_GL();
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		CHECK_ERROR_GL();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_ERROR_GL();

		VertexBufferBase::_count = (GLsizei)count;
	}

private:
	VertexBuffer(const VertexBuffer&) { }
	VertexBuffer& operator=(const VertexBuffer&) { return *this; }
};


#endif
