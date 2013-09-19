// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TEXTURE_H
#define TEXTURE_H

#if !TARGET_OS_IPHONE
#include <GL/glew.h>
#include <GL/gl.h>
//#include <OpenGL/gl.h>
#endif

#include "../resource.h"

class image;


struct texture
{
	GLuint id;

	texture();
	texture(const resource& r);
	texture(const image& image);
	~texture();

	void init();

	void load(const resource& r);
	void load(const image& image);

private:
	texture(const texture&) {}
	texture& operator=(const texture&) { return *this; }
};


#endif
