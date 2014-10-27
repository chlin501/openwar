// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef PatchShape_H
#define PatchShape_H

#include "VertexShape.h"
#include "Shape.h"

class PatchShape;


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



class PatchGlyphXX
{
	friend class PatchShape;
	PatchShape* _vertexBuffer;

public:
	typedef std::function<void(std::vector<Vertex_2f_2f>&)> RebuildType;

	RebuildType _rebuild;

	PatchGlyphXX() : _vertexBuffer(nullptr), _rebuild() { }
	PatchGlyphXX(RebuildType rebuild) : _vertexBuffer(nullptr), _rebuild(rebuild) { }

	~PatchGlyphXX();

private:
	PatchGlyphXX(const PatchGlyphXX&) { }
	PatchGlyphXX& operator=(PatchGlyphXX&) { return *this; }
};


class PatchGlyphX
{
	PatchGlyphXX _glyph;

public:
	bounds2f outer_xy;
	bounds2f inner_xy;
	bounds2f outer_uv;
	bounds2f inner_uv;

	PatchGlyphX() { }
	PatchGlyphX(TexturePatch tile, bounds2f bounds, glm::vec2 inset);

	void Reset();
	void Reset(TexturePatch tile, bounds2f bounds, glm::vec2 inset);

	PatchGlyphXX* GetGlyph();

protected:
	void generate(std::vector<Vertex_2f_2f>& vertices);
	void rectangle(std::vector<Vertex_2f_2f>& vertices, bounds2f xy, bounds2f uv);

private:
	PatchGlyphX(const PatchGlyphX&) { }
	PatchGlyphX& operator=(const PatchGlyphX&) { return *this; }
};


class PatchShape
{
	class PatchVertexBuffer : public VertexBuffer<Vertex_2f_2f>
	{
		PatchShape* _shape;
	public:
		PatchVertexBuffer(PatchShape* shape);
		virtual void Update();
	};

	friend class PatchGlyphXX;
	std::vector<PatchGlyphXX*> _glyphs;
	PatchVertexBuffer _vertices;

public:
	PatchShape();

	VertexBuffer<Vertex_2f_2f>* GetVertices();

	void ClearGlyphs();
	void AddGlyph(PatchGlyphXX* glyph);
	void RemoveGlyph(PatchGlyphXX* glyph);

private:
	void UpdateVertexBuffer();
};


#endif
