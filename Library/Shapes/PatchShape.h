// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef PatchShape_H
#define PatchShape_H

#include "VertexBuffer.h"


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


class PatchShape
{
	VertexGlyph<Vertex_2f_2f> _glyph;
public:
	typedef Vertex_2f_2f vertex_type;

	bounds2f outer_xy;
	bounds2f inner_xy;
	bounds2f outer_uv;
	bounds2f inner_uv;

	PatchShape() { }
	PatchShape(TexturePatch tile, bounds2f bounds, glm::vec2 inset);

	void Reset();
	void Reset(TexturePatch tile, bounds2f bounds, glm::vec2 inset);


	VertexGlyph<Vertex_2f_2f>* GetGlyph();

	void generate(std::vector<vertex_type>& vertices);
	void rectangle(std::vector<vertex_type>& vertices, bounds2f xy, bounds2f uv);
};


#endif
