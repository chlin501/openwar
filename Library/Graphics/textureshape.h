// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef textureshape_H
#define textureshape_H

#include "VertexBuffer.h"


struct texturetile
{
	bounds2f outer;
	bounds2f inner;

	texturetile();
	texturetile(bounds2f o, bounds2f i);
};

struct textureatlas
{
	glm::ivec2 size;

	textureatlas(int size_u, int size_v);
	texturetile tile(int u0, int v0, int size_u, int size_v, int insert_u, int inset_v);
};


class textureglyph
{
public:
	typedef Vertex_2f_2f vertex_type;

	bounds2f outer_xy;
	bounds2f inner_xy;
	bounds2f outer_uv;
	bounds2f inner_uv;

	textureglyph() { }
	textureglyph(texturetile tile, bounds2f bounds, glm::vec2 inset);

	vertexglyph2<glm::vec2, glm::vec2> glyph();

	void generate(std::vector<vertex_type>& vertices);
	void rectangle(std::vector<vertex_type>& vertices, bounds2f xy, bounds2f uv);
};


class textureshape : public vertexshape2<glm::vec2, glm::vec2>
{
public:
	textureshape();
	~textureshape();


};


#endif
