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


class Texture
{
public:
	GLuint id;

	explicit Texture(GraphicsContext* gc);
	virtual ~Texture();

	void init();

private:
	Texture(const Texture&) {}
	Texture& operator=(const Texture&) { return *this; }
};


class TextureResource : public Texture
{
public:
	TextureResource(GraphicsContext* gc, const resource& r);
	void load(GraphicsContext* gc, const resource& r);
};


class TextureXXX : public Texture
{
public:
	explicit TextureXXX(GraphicsContext* gc) : Texture(gc) { }
	TextureXXX(GraphicsContext* gc, const Image& image);
	TextureXXX(GraphicsContext* gc, SDL_Surface* surface);
	void load(const Image& image);
	void load(SDL_Surface* surface);
};


#endif
