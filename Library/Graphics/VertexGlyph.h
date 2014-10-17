#ifndef VertexGlyph_H
#define VertexGlyph_H

#include <functional>
#include <vector>

class Shape;
template <class _Vertex> class VertexBuffer;


class VertexGlyphBase
{
	Shape* _shape;

public:
	VertexGlyphBase(Shape* shape);
	~VertexGlyphBase();
};


template <class _Vertex>
class VertexGlyph : public VertexGlyphBase
{
	friend class VertexBuffer<_Vertex>;
	VertexBuffer<_Vertex>* _vertexBuffer;

public:
	typedef _Vertex VertexType;
	typedef std::function<void(std::vector<_Vertex>&)> RebuildType;

	RebuildType _rebuild;

	VertexGlyph() : VertexGlyphBase(nullptr), _vertexBuffer(nullptr), _rebuild() { }
	VertexGlyph(RebuildType rebuild) : VertexGlyphBase(nullptr), _vertexBuffer(nullptr), _rebuild(rebuild) { }

	~VertexGlyph()
	{
		if (_vertexBuffer != nullptr)
			_vertexBuffer->RemoveGlyph(this);
	}

private:
	VertexGlyph<VertexType>& operator=(VertexGlyph<VertexType>&) { return *this; }
};


#endif
