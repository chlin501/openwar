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


struct TextureSheet
{
	TextureAtlas* _textureAtlas;
	bounds2f _sheetBounds;

	TextureSheet(TextureAtlas* textureAtlas, int size_u, int size_v);
	TextureSheet(TextureAtlas* textureAtlas, const bounds2f& bounds);

	glm::vec2 MapCoord(int u, int v) const;

	TextureImage* NewTextureImage(int u0, int v0, int size_u, int size_v);
	TextureImage* NewTexturePatch(int u0, int v0, int size_u, int size_v, int insert_u, int inset_v);
};


class TextureAtlas : public Texture
{
	friend class TextureImage;
	friend class TextureSheet;

	GraphicsContext* _gc;
	Image* _textureAtlasImage;
	std::map<FontAdapter*, TextureFont*> _textureFonts;
	std::vector<TextureImage*> _textureImages;
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

	TextureImage* AddTextureImage(const Image& image, TextureImageType textureImageType);
	TextureSheet AddTextureSheet(const Image& image);
	TextureSheet GetTextureSheet(const bounds2f& bounds);

private:
	TextureImage* NewTextureImage(bool discardable, const bounds2f& inner, const bounds2f& outer);

private:
	TextureAtlas(const TextureAtlas&) : Texture(nullptr) { }
	TextureAtlas& operator=(const TextureAtlas&) { return *this; }
};


class TextureImage
{
	friend class TextureAtlas;
	friend class TexturePatch;
	TextureAtlas* _textureAtlas;
	bool _discardable;
	bool _discarded;

public:
	bounds2f _inner;
	bounds2f _outer;

	TextureImage();
	~TextureImage();

	bool IsDiscarded() const;

	bounds2f GetInnerBounds() const;
	bounds2f GetOuterBounds() const;
	bounds2f GetInnerUV() const;
	bounds2f GetOuterUV() const;

private:
	TextureImage(const TextureImage&) { }
	TextureImage& operator=(const TextureImage&) { return *this; }
};


#endif
