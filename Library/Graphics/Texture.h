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



#endif
