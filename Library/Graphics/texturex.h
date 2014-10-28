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


struct texturex
{
	GLuint id;

	explicit texturex(GraphicsContext* gc);
	texturex(GraphicsContext* gc, const resource& r);
	texturex(GraphicsContext* gc, const Image& image);
	texturex(GraphicsContext* gc, SDL_Surface* surface);

	~texturex();

	void init();

	void load(GraphicsContext* gc, const resource& r);
	void load(const Image& image);
	void load(SDL_Surface* surface);

private:
	texturex(const texturex&) {}
	texturex& operator=(const texturex&) { return *this; }
};


#endif
