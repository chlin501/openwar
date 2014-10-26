#ifndef VertexShape_H
#define VertexShape_H

#include "VertexBuffer.h"
#include <functional>
#include <vector>

template <class _Vertex> class VertexBuffer;


template <class _Vertex>
class VertexShape : public VertexBuffer<_Vertex>
{
public:
	typedef _Vertex VertexT;

	std::vector<VertexT> _vertices;

	VertexShape() { }

	void Reset(GLenum mode)
	{
		VertexBufferBase::_mode = mode;
		_vertices.clear();
	}

	void Clear()
	{
		_vertices.clear();
	}

	void AddVertex(const VertexT& vertex)
	{
		_vertices.push_back(vertex);
	}

	virtual void UpdateVBO(GLenum usage)
	{
		if (VertexBufferBase::_vbo == 0)
		{
			glGenBuffers(1, &this->_vbo);
			CHECK_ERROR_GL();
			if (VertexBufferBase::_vbo == 0)
				return;
		}

		GLsizeiptr size = sizeof(VertexT) * _vertices.size();
		const GLvoid* data = _vertices.data();

		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase::_vbo);
		CHECK_ERROR_GL();
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
		CHECK_ERROR_GL();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_ERROR_GL();

		VertexBufferBase::_count = (GLsizei)_vertices.size();
	}


	virtual const void* data() const
	{
		return _vertices.data();
	}

	virtual GLsizei count() const
	{
		return VertexBufferBase::_vbo != 0 ? VertexBufferBase::_count : (GLsizei)_vertices.size();
	}
};

typedef VertexShape<Vertex_2f> VertexShape_2f;
typedef VertexShape<Vertex_3f> VertexShape_3f;

typedef VertexShape<Vertex_2f_2f> VertexShape_2f_2f;
typedef VertexShape<Vertex_2f_4f> VertexShape_2f_4f;
typedef VertexShape<Vertex_3f_1f> VertexShape_3f_1f;
typedef VertexShape<Vertex_3f_2f> VertexShape_3f_2f;
typedef VertexShape<Vertex_3f_3f> VertexShape_3f_3f;
typedef VertexShape<Vertex_3f_4f> VertexShape_3f_4f;

typedef VertexShape<Vertex_3f_4f_1f> VertexShape_3f_4f_1f;
typedef VertexShape<Vertex_2f_2f_2f> VertexShape_2f_2f_2f;

typedef VertexShape<Vertex_3f_1f_2f_2f> VertexShape_3f_1f_2f_2f;


#endif
