#ifndef TextureAtlas_H
#define TextureAtlas_H

#include <vector>
#include <glm/glm.hpp>
#include "Algebra/bounds.h"
#include "Texture.h"

class Image;
class TextureImage;


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
	void LoadTextureFromSurface(SDL_Surface* surface);

	TextureImage* AddTextureImage(Image* image);

private:
	TextureAtlas(const TextureAtlas&) : Texture(nullptr) {}
	TextureAtlas& operator=(const TextureAtlas&) { return *this; }
};


class TextureImage
{
	friend class TextureAtlas;
	friend class TexturePatch;
	TextureAtlas* _textureAtlas;
	int _x;
	int _y;
	int _w;
	int _h;

	TextureImage();
public:
	virtual ~TextureImage();

	bounds2f GetBoundsUV() const;

private:
	TextureImage(const TextureImage&) { }
	TextureImage& operator=(const TextureImage&) { return *this; }
};


class TexturePatch : TextureImage
{
public:


private:
	TexturePatch(const TexturePatch&) { }
	TexturePatch& operator=(const TexturePatch&) { return *this; }
};


#endif
