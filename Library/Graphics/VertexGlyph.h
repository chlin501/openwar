#ifndef VertexGlyph_H
#define VertexGlyph_H

#include <functional>
#include <vector>


template <class _VertexT>
struct VertexGlyph
{
	typedef std::function<void(std::vector<_VertexT>&)> RebuildType;

	RebuildType _rebuild;

	VertexGlyph() : _rebuild() { }
	VertexGlyph(RebuildType rebuild) : _rebuild(rebuild) { }
};


#endif
