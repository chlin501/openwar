// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Image_h
#define Image_h

#include "GraphicsOpenGL.h"
#include <glm/glm.hpp>
#include "resource.h"

//#define OPENWAR_IMAGE_USE_SDL
#define OPENWAR_IMAGE_USE_COREGRAPHICS

#ifdef OPENWAR_IMAGE_USE_SDL
#include <SDL2/SDL.h>
#endif

#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
#import <CoreGraphics/CoreGraphics.h>
#endif


class Image
{
public:
#ifdef OPENWAR_IMAGE_USE_SDL
	SDL_Surface* _surface;
#endif
#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
	CGContextRef _context;
#endif

	int _width;
	int _height;
	unsigned char* _pixels;
	bool _owner;

public:
	Image();
	Image(int width, int height);
	~Image();

	void LoadFromResource(const resource& r);

#ifdef OPENWAR_IMAGE_USE_SDL
	SDL_Surface* GetSurface();
#endif

#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
	void LoadFromCGImage(CGImageRef image);
	CGContextRef GetCGContext() const;
#endif

	int GetWidth() const;
	int GetHeight() const;
	const void* GetPixels() const;

	glm::ivec2 size() const { return glm::ivec2(GetWidth(), GetHeight()); }
	glm::vec4 GetPixel(int x, int y) const;
	void SetPixel(int x, int y, glm::vec4 c);
	void PremultiplyAlpha();
};


#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
NSData* ConvertImageToTiff(Image* map);
Image* ConvertTiffToImage(NSData* data);
#endif


#endif
