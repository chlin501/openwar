#ifndef TextureAtlas_H
#define TextureAtlas_H

#include "Texture.h"


class TextureAtlas : public Texture
{
public:
	explicit TextureAtlas(GraphicsContext* gc) : Texture(gc) { }
	TextureAtlas(GraphicsContext* gc, const Image& image);
	TextureAtlas(GraphicsContext* gc, SDL_Surface* surface);
	void LoadTextureFromImage(const Image& image);
	void LoadTextureFromSurface(SDL_Surface* surface);
};


#endif
