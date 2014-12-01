// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Image_H
#define Image_H

#include <functional>
#include <glm/glm.hpp>
#include "resource.h"
#include "Algebra/bounds.h"


#ifdef OPENWAR_IMAGE_ENABLE_SDL
#include <SDL2/SDL.h>
#endif

#ifdef OPENWAR_IMAGE_ENABLE_COREGRAPHICS
#import <CoreGraphics/CoreGraphics.h>
#endif


class Image
{
#ifdef OPENWAR_IMAGE_ENABLE_SDL
	mutable SDL_Surface* _surface;
#endif
#ifdef OPENWAR_IMAGE_ENABLE_COREGRAPHICS
	mutable CGContextRef _context;
	mutable CGImageRef _image;
#endif

	int _width;
	int _height;
	float _pixelDensity;
	unsigned char* _pixels;
	bool _owner;

public:
	Image();
	Image(const Image& image);
	Image(int width, int height);
	~Image();

	float GetPixelDensity() const;
	void SetPixelDensity(float value);

	Image& LoadFromResource(const resource& r);

#ifdef OPENWAR_IMAGE_ENABLE_SDL
	void LoadFromSurface(SDL_Surface* surface);
	SDL_Surface* GetSurface() const;
#endif

#ifdef OPENWAR_IMAGE_ENABLE_COREGRAPHICS
	void LoadFromCGImage(CGImageRef image, float pixelDensity);

	CGContextRef GetCGContext() const;
	CGImageRef GetCGImage() const;
#endif

	int GetWidth() const;
	int GetHeight() const;
	const void* GetPixels() const;

	glm::ivec2 size() const { return glm::ivec2(GetWidth(), GetHeight()); }
	glm::vec4 GetPixel(int x, int y) const;
	void SetPixel(int x, int y, glm::vec4 c);

	bool IsGrayscale() const;

	void PremultiplyAlpha();
	void NormalizeAlpha();
	void ApplyBlurFilter(float radius);
	void ApplyPixelFilter(std::function<glm::vec4(glm::vec4)> filter);
	void ApplyCircleMask();

	void Copy(const Image& image, int x, int y);
	void Fill(const glm::vec4& color, const bounds2f& bounds);

private:
	Image& operator=(const Image&) { return *this; }
};


#ifdef OPENWAR_IMAGE_ENABLE_COREGRAPHICS
NSData* ConvertImageToTiff(Image* map);
Image* ConvertTiffToImage(NSData* data);
#endif


#endif
