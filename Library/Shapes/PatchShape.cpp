// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "PatchShape.h"



TexturePatchXXX::TexturePatchXXX() :
outer(0, 0, 1, 1),
inner(0, 0, 1, 1)
{
}


TexturePatchXXX::TexturePatchXXX(bounds2f o, bounds2f i) :
outer(o),
inner(i)
{
}


TexturePatchFactory::TexturePatchFactory(int size_u, int size_v) :
size(size_u, size_v)
{
}


TexturePatchXXX TexturePatchFactory::GetTexturePatch(int u0, int v0, int size_u, int size_v, int inset_u, int inset_v)
{
	bounds2f outer = bounds2f(u0, v0, u0 + size_u, v0 + size_v) / glm::vec2(size.x, size.y);
	bounds2f inner = outer.grow(-(float)inset_u / size.x, -(float)inset_v / size.y);
	return TexturePatchXXX(outer, inner);
}


PatchGlyph::PatchGlyph() :
	_patchShape(nullptr)
{

}


PatchGlyph::PatchGlyph(TexturePatchXXX tile, bounds2f bounds, glm::vec2 inset) :
	_patchShape(nullptr)
{
	outer_xy = bounds;
	inner_xy = bounds.grow(-inset.x, -inset.y);
	outer_uv = tile.outer;
	inner_uv = tile.inner;
}


PatchGlyph::~PatchGlyph()
{
	if (_patchShape != nullptr)
		_patchShape->RemoveGlyph(this);
}


void PatchGlyph::Reset()
{
	outer_xy = bounds2f();
	inner_xy = bounds2f();
	outer_uv = bounds2f();
	inner_uv = bounds2f();
}


void PatchGlyph::Reset(TexturePatchXXX tile, bounds2f bounds, glm::vec2 inset)
{
	outer_xy = bounds;
	inner_xy = bounds.grow(-inset.x, -inset.y);
	outer_uv = tile.outer;
	inner_uv = tile.inner;
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
	for (PatchGlyph* glyph : _glyphs)
		glyph->_patchShape = nullptr;
	_glyphs.clear();
}


void PatchShape::AddGlyph(PatchGlyph* glyph)
{
	if (glyph->_patchShape != nullptr)
		glyph->_patchShape->RemoveGlyph(glyph);
	glyph->_patchShape = this;
	_glyphs.push_back(glyph);
}


void PatchShape::RemoveGlyph(PatchGlyph* glyph)
{
	glyph->_patchShape = nullptr;
	_glyphs.erase(
		std::find(_glyphs.begin(), _glyphs.end(), glyph),
		_glyphs.end());
}


void PatchShape::UpdateVertexBuffer()
{
	static std::vector<Vertex_2f_2f> vertices;

	for (PatchGlyph* glyph : _glyphs)
		AppendShapeGlyph(vertices, glyph);

	_vertices.UpdateVBO(GL_TRIANGLES, vertices.data(), vertices.size());
	vertices.clear();
}


void PatchShape::AppendShapeGlyph(std::vector<Vertex_2f_2f>& vertices, PatchGlyph* patchGlyph)
{
	bounds2f ixy = patchGlyph->inner_xy;
	bounds2f oxy = patchGlyph->outer_xy;
	bounds2f iuv = patchGlyph->inner_uv;
	bounds2f ouv = patchGlyph->outer_uv;

	bool min_x = oxy.min.x < ixy.min.x;
	bool max_x = ixy.max.x < oxy.max.x;
	bool min_y = oxy.min.y < ixy.min.y;
	bool max_y = ixy.max.y < oxy.max.y;

	if (min_x && min_y)
		AppendRectangle(vertices,
			bounds2f(oxy.min.x, oxy.min.y, ixy.min.x, ixy.min.y),
			bounds2f(ouv.min.x, ouv.max.y, iuv.min.x, iuv.max.y));

	if (min_x)
		AppendRectangle(vertices,
			bounds2f(oxy.min.x, ixy.min.y, ixy.min.x, ixy.max.y),
			bounds2f(ouv.min.x, iuv.max.y, iuv.min.x, iuv.min.y));

	if (min_x && max_y)
		AppendRectangle(vertices,
			bounds2f(oxy.min.x, ixy.max.y, ixy.min.x, oxy.max.y),
			bounds2f(ouv.min.x, iuv.min.y, iuv.min.x, ouv.min.y));

	if (min_y)
		AppendRectangle(vertices,
			bounds2f(ixy.min.x, oxy.min.y, ixy.max.x, ixy.min.y),
			bounds2f(iuv.min.x, ouv.max.y, iuv.max.x, iuv.max.y));

	AppendRectangle(vertices,
		bounds2f(ixy.min.x, ixy.min.y, ixy.max.x, ixy.max.y),
		bounds2f(iuv.min.x, iuv.max.y, iuv.max.x, iuv.min.y));

	if (max_y)
		AppendRectangle(vertices,
			bounds2f(ixy.min.x, ixy.max.y, ixy.max.x, oxy.max.y),
			bounds2f(iuv.min.x, iuv.min.y, iuv.max.x, ouv.min.y));

	if (max_x && min_y)
		AppendRectangle(vertices,
			bounds2f(ixy.max.x, oxy.min.y, oxy.max.x, ixy.min.y),
			bounds2f(iuv.max.x, ouv.max.y, ouv.max.x, iuv.max.y));

	if (max_x)
		AppendRectangle(vertices,
			bounds2f(ixy.max.x, ixy.min.y, oxy.max.x, ixy.max.y),
			bounds2f(iuv.max.x, iuv.max.y, ouv.max.x, iuv.min.y));

	if (max_x && max_y)
		AppendRectangle(vertices,
			bounds2f(ixy.max.x, ixy.max.y, oxy.max.x, oxy.max.y),
			bounds2f(iuv.max.x, iuv.min.y, ouv.max.x, ouv.min.y));
}


void PatchShape::AppendRectangle(std::vector<Vertex_2f_2f>& vertices, bounds2f xy, bounds2f uv)
{
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.min.x, xy.min.y), glm::vec2(uv.min.x, uv.min.y)));
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.min.x, xy.max.y), glm::vec2(uv.min.x, uv.max.y)));
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.max.x, xy.max.y), glm::vec2(uv.max.x, uv.max.y)));
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.max.x, xy.max.y), glm::vec2(uv.max.x, uv.max.y)));
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.max.x, xy.min.y), glm::vec2(uv.max.x, uv.min.y)));
	vertices.push_back(Vertex_2f_2f(glm::vec2(xy.min.x, xy.min.y), glm::vec2(uv.min.x, uv.min.y)));
}

