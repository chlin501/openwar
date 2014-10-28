// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TEXTURE_H
#define TEXTURE_H

#include "GraphicsOpenGL.h"
#include "resource.h"
#include <SDL2/SDL.h>

class GraphicsContext;
class Image;


struct Texture
{
	GLuint id;

	explicit Texture(GraphicsContext* gc);
	Texture(GraphicsContext* gc, const resource& r);
	Texture(GraphicsContext* gc, const Image& image);
	Texture(GraphicsContext* gc, SDL_Surface* surface);

	~Texture();

	void init();

	void load(GraphicsContext* gc, const resource& r);
	void load(const Image& image);
	void load(SDL_Surface* surface);

private:
	Texture(const Texture&) {}
	Texture& operator=(const Texture&) { return *this; }
};


#endif
