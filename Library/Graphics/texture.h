// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TEXTURE_H
#define TEXTURE_H

#include "GraphicsOpenGL.h"
#include "resource.h"
#include <SDL2/SDL.h>

class Image;


struct texture
{
	GLuint id;

	texture();
	explicit texture(const resource& r);
	explicit texture(const Image& image);
	explicit texture(SDL_Surface* surface);

	~texture();

	void init();

	void load(const resource& r);
	void load(const Image& image);
	void load(SDL_Surface* surface);

private:
	texture(const texture&) {}
	texture& operator=(const texture&) { return *this; }
};


#endif
