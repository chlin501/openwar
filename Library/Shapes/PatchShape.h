// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef PatchShape_H
#define PatchShape_H

#include "VertexShape.h"
#include "Shape.h"

template <class _Vertex> class PatchShape;


struct TexturePatch
{
	bounds2f outer;
	bounds2f inner;

	TexturePatch();
	TexturePatch(bounds2f o, bounds2f i);
};

struct TexturePatchFactory
{
	glm::ivec2 size;

	TexturePatchFactory(int size_u, int size_v);
	TexturePatch GetTexturePatch(int u0, int v0, int size_u, int size_v, int insert_u, int inset_v);
};



template <class _Vertex>
class PatchGlyphXX
{
	friend class PatchShape<_Vertex>;
	PatchShape<_Vertex>* _vertexBuffer;

public:
	typedef _Vertex VertexType;
	typedef std::function<void(std::vector<_Vertex>&)> RebuildType;

	RebuildType _rebuild;

	PatchGlyphXX() : _vertexBuffer(nullptr), _rebuild() { }
	PatchGlyphXX(RebuildType rebuild) : _vertexBuffer(nullptr), _rebuild(rebuild) { }

	~PatchGlyphXX()
	{
		if (_vertexBuffer != nullptr)
			_vertexBuffer->RemoveGlyph(this);
	}

private:
	PatchGlyphXX(const PatchGlyphXX<VertexType>&) { }
	PatchGlyphXX<VertexType>& operator=(PatchGlyphXX<VertexType>&) { return *this; }
};


class PatchGlyphX
{
	PatchGlyphXX<Vertex_2f_2f> _glyph;

public:
	bounds2f outer_xy;
	bounds2f inner_xy;
	bounds2f outer_uv;
	bounds2f inner_uv;

	PatchGlyphX() { }
	PatchGlyphX(TexturePatch tile, bounds2f bounds, glm::vec2 inset);

	void Reset();
	void Reset(TexturePatch tile, bounds2f bounds, glm::vec2 inset);

	PatchGlyphXX<Vertex_2f_2f>* GetGlyph();

protected:
	void generate(std::vector<Vertex_2f_2f>& vertices);
	void rectangle(std::vector<Vertex_2f_2f>& vertices, bounds2f xy, bounds2f uv);

private:
	PatchGlyphX(const PatchGlyphX&) { }
	PatchGlyphX& operator=(const PatchGlyphX&) { return *this; }
};


template <class _Vertex>
class PatchShape : public VertexShape<_Vertex>
{
	friend class PatchGlyphXX<_Vertex>;
	std::vector<PatchGlyphXX<_Vertex>*> _glyphs;

public:
	typedef _Vertex VertexT;

	PatchShape() { }

	void ClearGlyphs()
	{
		for (PatchGlyphXX<VertexT>* glyph : _glyphs)
			glyph->_vertexBuffer = nullptr;
		_glyphs.clear();
	}

	void AddGlyph(PatchGlyphXX<VertexT>* glyph)
	{
		if (glyph->_vertexBuffer != nullptr)
			glyph->_vertexBuffer->RemoveGlyph(glyph);
		glyph->_vertexBuffer = this;
		_glyphs.push_back(glyph);
	}

	void RemoveGlyph(PatchGlyphXX<VertexT>* glyph)
	{
		glyph->_vertexBuffer = nullptr;
		_glyphs.erase(
			std::find(_glyphs.begin(), _glyphs.end(), glyph),
			_glyphs.end());
	}

	VertexBuffer<VertexT>& UpdateVBOFromGlyphs()
	{
		VertexShape<VertexT>::_vertices.clear();
		for (PatchGlyphXX<VertexT>* glyph : _glyphs)
		{
			if (glyph->_rebuild)
				glyph->_rebuild(VertexShape<VertexT>::_vertices);
		}
		this->UpdateVBO(GL_STATIC_DRAW);
		return *this;
	}
};


#endif
