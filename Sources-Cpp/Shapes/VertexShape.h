// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef VertexShape_H
#define VertexShape_H

#include "Graphics/VertexBuffer.h"
#include <functional>
#include <vector>

template <class _Vertex> class VertexBuffer;


template <class _Vertex>
class VertexShape : public VertexBuffer<_Vertex>
{
	std::vector<_Vertex> _vertices;
	bool _dirty{};

public:
	VertexShape() { }

	const std::vector<_Vertex>& GetVertices() const
	{
		return _vertices;
	}

	std::vector<_Vertex>& GetMutableVertices()
	{
		_dirty = true;
		return _vertices;
	}

	virtual void Update()
	{
		if (_dirty)
		{
			VertexBuffer<_Vertex>::UpdateVBO(VertexBufferBase::_mode, _vertices.data(), _vertices.size());
			_dirty = false;
		}
	}

	void Reset(GLenum mode)
	{
		VertexBufferBase::_mode = mode;
		_vertices.clear();
		_dirty = true;
	}

	void Clear()
	{
		_vertices.clear();
		_dirty = true;
	}

	void AddVertex(const _Vertex& vertex)
	{
		_vertices.push_back(vertex);
		_dirty = true;
	}
};


using VertexShape_2f = VertexShape<Vertex_2f>;
using VertexShape_3f = VertexShape<Vertex_3f>;

using VertexShape_2f_2f = VertexShape<Vertex_2f_2f>;
using VertexShape_2f_4f = VertexShape<Vertex_2f_4f>;
using VertexShape_3f_1f = VertexShape<Vertex_3f_1f>;
using VertexShape_3f_2f = VertexShape<Vertex_3f_2f>;
using VertexShape_3f_3f = VertexShape<Vertex_3f_3f>;
using VertexShape_3f_4f = VertexShape<Vertex_3f_4f>;

using VertexShape_3f_4f_1f = VertexShape<Vertex_3f_4f_1f>;
using VertexShape_2f_2f_2f = VertexShape<Vertex_2f_2f_2f>;

using VertexShape_3f_1f_2f_2f = VertexShape<Vertex_3f_1f_2f_2f>;


#endif
