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


struct texture
{
	GLuint id;

	explicit texture(GraphicsContext* gc);
	texture(GraphicsContext* gc, const resource& r);
	texture(GraphicsContext* gc, const Image& image);
	texture(GraphicsContext* gc, SDL_Surface* surface);

	~texture();

	void init();

	void load(GraphicsContext* gc, const resource& r);
	void load(const Image& image);
	void load(SDL_Surface* surface);

private:
	texture(const texture&) {}
	texture& operator=(const texture&) { return *this; }
};


#endif
