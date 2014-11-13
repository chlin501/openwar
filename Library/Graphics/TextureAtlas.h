// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TextureAtlas_H
#define TextureAtlas_H

#include <map>
#include <vector>
#include "Algebra/bounds.h"
#include "Texture.h"
#include "Image.h"

struct FontDescriptor;
class FontAdapter;
class TextureAtlas;
class TextureFont;
class TextureImage;


enum class TextureImageType { Permanent, Discardable };


struct BorderInset
{
	glm::vec2 min, max;
	BorderInset() { }
	BorderInset(float d) : min(d), max(d) { }

	BorderInset& MinX(float d) { min.x = d; return *this; }
	BorderInset& MaxX(float d) { max.x = d; return *this; }
	BorderInset& MinY(float d) { min.y = d; return *this; }
	BorderInset& MaxY(float d) { max.y = d; return *this; }

	BorderInset& X(float d) { min.x = max.x = d; return *this; }
	BorderInset& Y(float d) { min.y = max.y = d; return *this; }
};


struct BorderBounds
{
	bounds2f inner;
	bounds2f outer;

	BorderBounds() { }
	explicit BorderBounds(bounds2f bounds) : inner(bounds), outer(bounds) { }
	BorderBounds(float x, float y, float sx, float sy) : inner(x, y, x + sx, y + sy), outer(inner) { }

	BorderBounds& Inset(const BorderInset& inset) { inner.min = outer.min + inset.min; inner.max = outer.max - inset.max; return *this; }

	BorderBounds& InsetMinX(float d) {inner.min.x = outer.min.x + d; return *this; }
	BorderBounds& InsetMaxX(float d) {inner.max.x = outer.max.x - d; return *this; }

	BorderBounds& InsetMinY(float d) {inner.min.y = outer.min.y + d; return *this; }
	BorderBounds& InsetMaxY(float d) {inner.max.y = outer.max.y - d; return *this; }

	BorderBounds& OutsetMinX(float d) {outer.min.x = inner.min.x - d; return *this; }
	BorderBounds& OutsetMaxX(float d) {outer.max.x = inner.max.x + d; return *this; }

	BorderBounds& OutsetMinY(float d) {outer.min.y = inner.min.y - d; return *this; }
	BorderBounds& OutsetMaxY(float d) {outer.max.y = inner.max.y + d; return *this; }

	BorderBounds& InsetX(float d) { InsetMinX(d); InsetMaxX(d); return *this; }
	BorderBounds& InsetY(float d) { InsetMinY(d); InsetMaxY(d); return *this; }

	BorderBounds& OutsetX(float d) { OutsetMinX(d); OutsetMaxX(d); return *this; }
	BorderBounds& OutsetY(float d) { OutsetMinY(d); OutsetMaxY(d); return *this; }

	BorderBounds& Inset(float d) { InsetX(d); InsetY(d); return *this; }
	BorderBounds& Outset(float d) { OutsetX(d); OutsetY(d); return *this; }

	BorderBounds& Center() { inner = bounds2f(outer.center()); return *this; }
	BorderBounds& CenterX() { inner.min.x = inner.max.x = outer.x().center(); return *this; }
	BorderBounds& CenterY() { inner.min.y = inner.max.y = outer.y().center(); return *this; }
	BorderBounds& CenterMinX() { inner.min.x = outer.x().center(); return *this; }
	BorderBounds& CenterMaxX() { inner.max.x = outer.x().center(); return *this; }
	BorderBounds& CenterMinY() { inner.min.y = outer.y().center(); return *this; }
	BorderBounds& CenterMaxY() { inner.max.y = outer.y().center(); return *this; }
};


struct TextureSheet
{
	TextureAtlas* _textureAtlas;
	bounds2f _sheetBounds;

	TextureSheet(TextureAtlas* textureAtlas, int size_u, int size_v);
	TextureSheet(TextureAtlas* textureAtlas, const bounds2f& bounds);

	glm::vec2 MapCoord(int u, int v) const;

	std::shared_ptr<TextureImage> NewTextureImage(const BorderBounds& bounds);
};


class TextureAtlas : public Texture
{
	friend class TextureImage;
	friend class TextureSheet;

	GraphicsContext* _gc;
	Image* _textureAtlasImage;
	std::map<FontAdapter*, TextureFont*> _textureFonts;
	std::vector<std::shared_ptr<TextureImage>> _textureImages;
	glm::ivec2 _permamentPos;
	int _permanentHeight;
	glm::ivec2 _discardablePos;
	int _discardableHeight;
	bool _dirty;

public:
	explicit TextureAtlas(GraphicsContext* gc);

	GraphicsContext* GetGraphicsContext() const;

	TextureFont* GetTextureFont(const FontDescriptor& fontDescriptor);

	void LoadAtlasFromResource(const resource& r);
	void LoadTextureFromImage(const Image& image);

#ifdef OPENWAR_IMAGE_USE_SDL
	void LoadTextureFromSurface(SDL_Surface* surface);
#endif

	virtual void UpdateTexture();

	std::shared_ptr<TextureImage> AddTextureImage(const Image& image, TextureImageType textureImageType);
	TextureSheet AddTextureSheet(const Image& image);
	TextureSheet GetTextureSheet(const bounds2f& bounds);

private:
	std::shared_ptr<TextureImage> NewTextureImage(const BorderBounds& bounds, bool discardable);
	void DiscardTextureImages();

private:
	TextureAtlas(const TextureAtlas&) : Texture(nullptr) { }
	TextureAtlas& operator=(const TextureAtlas&) { return *this; }
};


class TextureImage
{
	friend class TextureAtlas;
	friend class TexturePatch;
	TextureAtlas* _textureAtlas;
	BorderBounds _bounds;
	bool _discardable;
	bool _discarded;

public:
	TextureImage();

	bool IsDiscarded() const;

	void SetBounds(const BorderBounds& value);

	BorderBounds GetBounds() const;
	BorderBounds GetCoords() const;

private:
	TextureImage(const TextureImage&) { }
	TextureImage& operator=(const TextureImage&) { return *this; }
};


#endif
