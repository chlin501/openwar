#ifndef TextureAtlas_H
#define TextureAtlas_H

#include <vector>
#include <glm/glm.hpp>
#include "Algebra/bounds.h"
#include "Texture.h"
#include "Image.h"

class TextureImage;


/*enum class TextureImageType
{
	Permanent,
	Discardable,
};*/


class TextureAtlas : public Texture
{
	friend class TextureImage;
	Image* _image;
	std::vector<TextureImage*> _images;
	int _currentX;
	int _currentY;
	int _nextY;

public:
	explicit TextureAtlas(GraphicsContext* gc);

	void LoadTextureFromImage(const Image& image);

#ifdef OPENWAR_IMAGE_USE_SDL
	void LoadTextureFromSurface(SDL_Surface* surface);
#endif

	void UpdateTextureAtlas();

	TextureImage* AddTextureImage(Image* image);

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

	bounds2f GetInnerBounds() const;
	bounds2f GetOuterBounds() const;
	bounds2f GetInnerUV() const;
	bounds2f GetOuterUV() const;

private:
	TextureImage(const TextureImage&) { }
	TextureImage& operator=(const TextureImage&) { return *this; }
};


#endif
