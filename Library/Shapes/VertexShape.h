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


	virtual void Update()
	{
		VertexBuffer<_Vertex>::UpdateVBO(VertexBufferBase::_mode, _vertices.data(), _vertices.size());
	}


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
