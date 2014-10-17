// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Image_h
#define Image_h

#include "GraphicsOpenGL.h"
#include <glm/glm.hpp>


#ifdef OPENWAR_USE_SDL
#include <SDL2/SDL.h>
#else
#import <CoreGraphics/CoreGraphics.h>
#endif

#include "resource.h"


class Image
{
#ifdef OPENWAR_USE_SDL
	SDL_Surface* _surface;
#else
	CGContextRef _context;
	size_t _width;
	size_t _height;
	GLubyte* _data;
#endif
	GLenum _format;

public:
	Image(int width, int height);
	explicit Image(const resource& r);

#ifndef OPENWAR_USE_SDL
	Image(CGImageRef image);
	CGContextRef CGContext() const { return _context; }
#endif

	~Image();

#ifdef OPENWAR_USE_SDL
	SDL_Surface* get_surface() { return _surface; }
#endif

#ifdef OPENWAR_USE_SDL
	GLsizei width() const { return (GLsizei)_surface->w; }
	GLsizei height() const { return (GLsizei)_surface->h; }
	GLenum format() const { return _format; }
	const GLvoid* pixels() const { return _surface->pixels; }
	glm::ivec2 size() const { return glm::ivec2(_surface->w, _surface->h); }
#else
	GLsizei width() const { return (GLsizei)_width; }
	GLsizei height() const { return (GLsizei)_height; }
	GLenum format() const { return _format; }
	const GLvoid* pixels() const { return _data; }
	glm::ivec2 size() const { return glm::ivec2(_width, _height); }
#endif

	glm::vec4 get_pixel(int x, int y) const;
	void set_pixel(int x, int y, glm::vec4 c);

	void premultiply_alpha();

private:
	void init_data_context();
};


#ifndef OPENWAR_USE_SDL
NSData* ConvertImageToTiff(Image* map);
Image* ConvertTiffToImage(NSData* data);
#endif


#endif
