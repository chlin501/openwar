#ifndef VertexGlyph_H
#define VertexGlyph_H

#include <functional>
#include <vector>


template <class _Vertex> class VertexBuffer;


template <class _Vertex>
class VertexGlyph
{
	friend class VertexBuffer<_Vertex>;
	VertexBuffer<_Vertex>* _vertexBuffer;

public:
	typedef std::function<void(std::vector<_Vertex>&)> RebuildType;

	RebuildType _rebuild;

	VertexGlyph() : _vertexBuffer(nullptr), _rebuild() { }
	VertexGlyph(RebuildType rebuild) : _vertexBuffer(nullptr), _rebuild(rebuild) { }

	~VertexGlyph()
	{
		if (_vertexBuffer != nullptr)
			_vertexBuffer->RemoveGlyph(this);
	}
};


#endif
