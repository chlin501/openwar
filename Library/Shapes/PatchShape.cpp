// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "PatchShape.h"



TexturePatch::TexturePatch() :
outer(0, 0, 1, 1),
inner(0, 0, 1, 1)
{
}


TexturePatch::TexturePatch(bounds2f o, bounds2f i) :
outer(o),
inner(i)
{
}


TexturePatchFactory::TexturePatchFactory(int size_u, int size_v) :
size(size_u, size_v)
{
}


TexturePatch TexturePatchFactory::GetTexturePatch(int u0, int v0, int size_u, int size_v, int inset_u, int inset_v)
{
	bounds2f outer = bounds2f(u0, v0, u0 + size_u, v0 + size_v) / glm::vec2(size.x, size.y);
	bounds2f inner = outer.grow(-(float)inset_u / size.x, -(float)inset_v / size.y);
	return TexturePatch(outer, inner);
}


PatchGlyphX::PatchGlyphX(TexturePatch tile, bounds2f bounds, glm::vec2 inset)
{
	outer_xy = bounds;
	inner_xy = bounds.grow(-inset.x, -inset.y);
	outer_uv = tile.outer;
	inner_uv = tile.inner;
}


void PatchGlyphX::Reset()
{
	outer_xy = bounds2f();
	inner_xy = bounds2f();
	outer_uv = bounds2f();
	inner_uv = bounds2f();
}


void PatchGlyphX::Reset(TexturePatch tile, bounds2f bounds, glm::vec2 inset)
{
	outer_xy = bounds;
	inner_xy = bounds.grow(-inset.x, -inset.y);
	outer_uv = tile.outer;
	inner_uv = tile.inner;
}


PatchGlyphXX* PatchGlyphX::GetGlyph()
{
	_glyph._rebuild = [this](std::vector<Vertex_2f_2f>& vertices) {
		generate(vertices);
	};
	return &_glyph;
}


void PatchGlyphX::generate(std::vector<Vertex_2f_2f>& vertices)
{
	bool min_x = outer_xy.min.x < inner_xy.min.x;
	bool max_x = inner_xy.max.x < outer_xy.max.x;
	bool min_y = outer_xy.min.y < inner_xy.min.y;
	bool max_y = inner_xy.max.y < outer_xy.max.y;

	if (min_x && min_y)
		rectangle(vertices,
			bounds2f(outer_xy.min.x, outer_xy.min.y, inner_xy.min.x, inner_xy.min.y),
			bounds2f(outer_uv.min.x, outer_uv.max.y, inner_uv.min.x, inner_uv.max.y));

	if (min_x)
		rectangle(vertices,
			bounds2f(outer_xy.min.x, inner_xy.min.y, inner_xy.min.x, inner_xy.max.y),
			bounds2f(outer_uv.min.x, inner_uv.max.y, inner_uv.min.x, inner_uv.min.y));

	if (min_x && max_y)
		rectangle(vertices,
			bounds2f(outer_xy.min.x, inner_xy.max.y, inner_xy.min.x, outer_xy.max.y),
			bounds2f(outer_uv.min.x, inner_uv.min.y, inner_uv.min.x, outer_uv.min.y));

	if (min_y)
		rectangle(vertices,
			bounds2f(inner_xy.min.x, outer_xy.min.y, inner_xy.max.x, inner_xy.min.y),
			bounds2f(inner_uv.min.x, outer_uv.max.y, inner_uv.max.x, inner_uv.max.y));

	rectangle(vertices,
		bounds2f(inner_xy.min.x, inner_xy.min.y, inner_xy.max.x, inner_xy.max.y),
		bounds2f(inner_uv.min.x, inner_uv.max.y, inner_uv.max.x, inner_uv.min.y));

	if (max_y)
		rectangle(vertices,
			bounds2f(inner_xy.min.x, inner_xy.max.y, inner_xy.max.x, outer_xy.max.y),
			bounds2f(inner_uv.min.x, inner_uv.min.y, inner_uv.max.x, outer_uv.min.y));

	if (max_x && min_y)
		rectangle(vertices,
			bounds2f(inner_xy.max.x, outer_xy.min.y, outer_xy.max.x, inner_xy.min.y),
			bounds2f(inner_uv.max.x, outer_uv.max.y, outer_uv.max.x, inner_uv.max.y));

	if (max_x)
		rectangle(vertices,
			bounds2f(inner_xy.max.x, inner_xy.min.y, outer_xy.max.x, inner_xy.max.y),
			bounds2f(inner_uv.max.x, inner_uv.max.y, outer_uv.max.x, inner_uv.min.y));

	if (max_x && max_y)
		rectangle(vertices,
			bounds2f(inner_xy.max.x, inner_xy.max.y, outer_xy.max.x, outer_xy.max.y),
			bounds2f(inner_uv.max.x, inner_uv.min.y, outer_uv.max.x, outer_uv.min.y));
}


void PatchGlyphX::rectangle(std::vector<Vertex_2f_2f>& vertices, bounds2f xy, bounds2f uv)
{
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.min.x, xy.min.y), glm::vec2(uv.min.x, uv.min.y)));
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.min.x, xy.max.y), glm::vec2(uv.min.x, uv.max.y)));
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.max.x, xy.max.y), glm::vec2(uv.max.x, uv.max.y)));
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.max.x, xy.max.y), glm::vec2(uv.max.x, uv.max.y)));
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.max.x, xy.min.y), glm::vec2(uv.max.x, uv.min.y)));
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.min.x, xy.min.y), glm::vec2(uv.min.x, uv.min.y)));
}


PatchGlyphXX::~PatchGlyphXX()
{
	if (_vertexBuffer != nullptr)
		_vertexBuffer->RemoveGlyph(this);
}


PatchShape::PatchVertexBuffer::PatchVertexBuffer(PatchShape* shape) :
	_shape(shape)
{
	_mode = GL_TRIANGLES;
}


void PatchShape::PatchVertexBuffer::Update()
{
	_shape->UpdateVertexBuffer();
}


PatchShape::PatchShape() :
	_vertices(this)
{
}


VertexBuffer<Vertex_2f_2f>* PatchShape::GetVertices()
{
	return &_vertices;
}


void PatchShape::ClearGlyphs()
{
	for (PatchGlyphXX* glyph : _glyphs)
		glyph->_vertexBuffer = nullptr;
	_glyphs.clear();
}


void PatchShape::AddGlyph(PatchGlyphXX* glyph)
{
	if (glyph->_vertexBuffer != nullptr)
		glyph->_vertexBuffer->RemoveGlyph(glyph);
	glyph->_vertexBuffer = this;
	_glyphs.push_back(glyph);
}


void PatchShape::RemoveGlyph(PatchGlyphXX* glyph)
{
	glyph->_vertexBuffer = nullptr;
	_glyphs.erase(
		std::find(_glyphs.begin(), _glyphs.end(), glyph),
		_glyphs.end());
}


void PatchShape::UpdateVertexBuffer()
{
	static std::vector<Vertex_2f_2f> vertices;

	for (PatchGlyphXX* glyph : _glyphs)
		if (glyph->_rebuild)
			glyph->_rebuild(vertices);

	_vertices.UpdateVBO(GL_TRIANGLES, vertices.data(), vertices.size());
	vertices.clear();
}
