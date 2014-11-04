#ifndef TextureAtlas_H
#define TextureAtlas_H

#include <vector>
#include <glm/glm.hpp>
#include "Algebra/bounds.h"
#include "Texture.h"
#include "Image.h"

class TextureAtlas;
class TextureImage;
class TextureFont;
struct FontDescriptor;



/*enum class TextureImageType
{
	Permanent,
	Discardable,
};*/


struct TextureSheet
{
	TextureAtlas* _textureAtlas;
	bounds2f _sheetBounds;

	TextureSheet(TextureAtlas* textureAtlas, int size_u, int size_v);
	TextureSheet(TextureAtlas* textureAtlas, const bounds2f& bounds);

	TextureImage* GetTextureImage(int u0, int v0, int size_u, int size_v);
	TextureImage* GetTexturePatch(int u0, int v0, int size_u, int size_v, int insert_u, int inset_v);
};


class TextureAtlas : public Texture
{
	friend class TextureImage;
	GraphicsContext* _gc;
	Image* _image;
	std::vector<TextureFont*> _textureFonts;
	std::vector<TextureImage*> _images;
	int _currentX;
	int _currentY;
	int _nextY;
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

	TextureImage* AddTextureImage(const Image& image);
	TextureImage* GetTextureImage(const bounds2f& inner, const bounds2f& outer);
	TextureSheet AddTextureSheet(const Image& image);

private:
	TextureAtlas(const TextureAtlas&) : Texture(nullptr) { }
	TextureAtlas& operator=(const TextureAtlas&) { return *this; }
};


class TextureImage
{
	friend class TextureAtlas;
	friend class TexturePatch;
	TextureAtlas* _textureAtlas;

public:
	bounds2f _inner;
	bounds2f _outer;

	TextureImage();
	~TextureImage();

	TextureAtlas* GetTextureAtlas() const;

	bounds2f GetInnerBounds() const;
	bounds2f GetOuterBounds() const;
	bounds2f GetInnerUV() const;
	bounds2f GetOuterUV() const;

private:
	TextureImage(const TextureImage&) { }
	TextureImage& operator=(const TextureImage&) { return *this; }
};


#endif
