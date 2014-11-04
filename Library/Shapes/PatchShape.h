// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef PatchShape_H
#define PatchShape_H

#include "VertexShape.h"

class PatchShape;
class TextureAtlas;
class TextureImage;


struct TextureSheet
{
	TextureAtlas* _textureAtlas;
	glm::ivec2 size;

	TextureSheet(TextureAtlas* textureAtlas, int size_u, int size_v);

	TextureImage* GetTextureImage(int u0, int v0, int size_u, int size_v);
	TextureImage* GetTexturePatch(int u0, int v0, int size_u, int size_v, int insert_u, int inset_v);
};


class PatchGlyph
{
	friend class PatchShape;
	PatchShape* _patchShape;

public:
	bounds2f outer_xy;
	bounds2f inner_xy;
	bounds2f outer_uv;
	bounds2f inner_uv;

	PatchGlyph();
	PatchGlyph(TextureImage* tile, bounds2f bounds, glm::vec2 inset);
	~PatchGlyph();

	void Reset();
	void Reset(TextureImage* tile, bounds2f bounds, glm::vec2 inset);

private:
	PatchGlyph(const PatchGlyph&) { }
	PatchGlyph& operator=(const PatchGlyph&) { return *this; }
};


class PatchShape
{
	friend class PatchGlyph;

	class PatchVertexBuffer : public VertexBuffer<Vertex_2f_2f>
	{
		PatchShape* _shape;
	public:
		PatchVertexBuffer(PatchShape* shape);
		virtual void Update();
	};

	PatchVertexBuffer _vertices;
	std::vector<PatchGlyph*> _glyphs;

public:
	PatchShape();

	VertexBuffer<Vertex_2f_2f>* GetVertices();

	void ClearGlyphs();
	void AddGlyph(PatchGlyph* glyph);
	void RemoveGlyph(PatchGlyph* glyph);

private:
	void UpdateVertexBuffer();
	void AppendShapeGlyph(std::vector<Vertex_2f_2f>& vertices, PatchGlyph* shapeGlyph);
	void AppendRectangle(std::vector<Vertex_2f_2f>& vertices, bounds2f xy, bounds2f uv);
};


#endif
