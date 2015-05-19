// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef VertexBuffer_H
#define VertexBuffer_H

#include "Algebra/bounds.h"
#include "GraphicsContext.h"
#include "Vertex.h"


class VertexBufferBase
{
	friend class RenderCallBase;
public:
	GLenum _mode{};
protected:
	GLuint _vbo{};
	GLsizei _count{};

public:
	VertexBufferBase();
	virtual ~VertexBufferBase();

	VertexBufferBase(VertexBufferBase&&);
	VertexBufferBase& operator=(VertexBufferBase&&);

	virtual void Update() = 0;
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

		if (_vbo == 0)
		{
			glGenBuffers(1, &this->_vbo);
			CHECK_ERROR_GL();
			if (_vbo == 0)
				return;
		}

		GLsizeiptr size = static_cast<GLsizeiptr>(sizeof(VertexT) * count);
		const GLvoid* data = static_cast<const GLvoid*>(vertices);

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		CHECK_ERROR_GL();
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		CHECK_ERROR_GL();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_ERROR_GL();

		_count = (GLsizei)count;
	}
};


#endif
