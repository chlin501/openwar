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
#define OPENWAR_USE_COREGRAPHICS
#import <CoreGraphics/CoreGraphics.h>
#endif

#include "resource.h"

class GraphicsContext;


class Image
{
public:
	virtual ~Image();

	virtual GLsizei width() const = 0;
	virtual GLsizei height() const = 0;
	virtual GLenum format() const = 0;
	virtual const GLvoid* pixels() const = 0;

	glm::ivec2 size() const { return glm::ivec2(width(), height()); }
	glm::vec4 GetPixel(int x, int y) const;
	void SetPixel(int x, int y, glm::vec4 c);
	void PremultiplyAlpha();
};


#ifdef OPENWAR_USE_COREGRAPHICS
class ImageCG : public Image
{
	CGContextRef _context;
	size_t _width;
	size_t _height;
	GLubyte* _data;
	GLenum _format;

public:
	ImageCG(int width, int height);
	ImageCG(GraphicsContext* gc, const resource& r);
	explicit ImageCG(CGImageRef image);
	virtual ~ImageCG();

	CGContextRef CGContext() const;

	virtual GLsizei width() const;
	virtual GLsizei height() const;
	virtual GLenum format() const;
	virtual const GLvoid* pixels() const;

private:
	void InitCGContext();
};
#endif


#ifdef OPENWAR_USE_SDL
class ImageSDL : public Image
{
	SDL_Surface* _surface;
	GLenum _format;

public:
	ImageSDL(int width, int height);
	ImageSDL(GraphicsContext* gc, const resource& r);
	virtual ~ImageSDL();

	SDL_Surface* get_surface() { return _surface; }

	GLsizei width() const { return (GLsizei)_surface->w; }
	GLsizei height() const { return (GLsizei)_surface->h; }
	GLenum format() const { return _format; }
	const GLvoid* pixels() const { return _surface->pixels; }
};
#endif


#ifdef OPENWAR_USE_COREGRAPHICS
typedef ImageCG MutableImageBase;
typedef ImageCG ResourceImageBase;
#else
typedef ImageSDL MutableImageBase;
typedef ImageSDL ResourceImageBase;
#endif


class MutableImage : public MutableImageBase
{
public:
	MutableImage(int width, int height) : MutableImageBase(width, height) { }
};


class ResourceImage : public ResourceImageBase
{
public:
	ResourceImage(GraphicsContext* gc, const resource& r) : MutableImageBase(gc, r) { }
};


#ifdef OPENWAR_USE_COREGRAPHICS
NSData* ConvertImageToTiff(Image* map);
Image* ConvertTiffToImage(NSData* data);
#endif


#endif
