// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Image.h"
#include "bounds.h"
#include "GraphicsContext.h"

#ifdef OPENWAR_IMAGE_USE_SDL
#include <SDL2_image/SDL_image.h>
#endif

#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif
#endif



Image::Image() :
#ifdef OPENWAR_IMAGE_USE_SDL
	_surface(nullptr),
#endif
#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
	_context(nil),
#endif
	_width(0),
	_height(0),
	_pixels(nullptr),
	_owner(false)
{
}


Image::Image(int width, int height) :
#ifdef OPENWAR_IMAGE_USE_SDL
	_surface(nullptr),
#endif
#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
	_context(nil),
#endif
	_width(width),
	_height(height),
	_pixels(nullptr),
	_owner(false)
{
	_pixels = (unsigned char*) calloc(_width * _height * 4, sizeof(unsigned char));
	_owner = true;

#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	_context = CGBitmapContextCreate(_pixels, _width, _height, 8, _width * 4, colorSpace, kCGImageAlphaPremultipliedLast);
	CGColorSpaceRelease(colorSpace);
#endif

#ifdef OPENWAR_IMAGE_USE_SDL
	_surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
#endif
}


void Image::LoadFromResource(const resource& r)
{
#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
#if TARGET_OS_IPHONE

	NSString* name = [NSString stringWithFormat:@"%@%@", [NSString stringWithUTF8String:r.name()], [NSString stringWithUTF8String:r.type()]];
	UIImage* image = [UIImage imageNamed:name];
	if (image != nil)
	{
		LoadFromCGImage(image.CGImage);
	}
	else
	{
		NSLog(@"image not found: %@", name);
	}

#else

	NSImage* image = [[NSImage alloc] initWithContentsOfFile:[NSString stringWithUTF8String:r.path()]];
	if (image != nil)
	{
		LoadFromCGImage([image CGImageForProposedRect:nil context:nil hints:nil]);
		[image release];
	}
	else
	{
		NSString* name = [NSString stringWithFormat:@"%@%@", [NSString stringWithUTF8String:r.name()], [NSString stringWithUTF8String:r.type()]];
		NSLog(@"image not found: %@", name);
	}

	return;

#endif


#endif

#ifdef OPENWAR_IMAGE_USE_SDL

	_surface = IMG_Load(r.path());
	if (_surface->format->format != SDL_PIXELFORMAT_ABGR8888)
	{
		SDL_Surface* surface = SDL_ConvertSurfaceFormat(_surface, SDL_PIXELFORMAT_ABGR8888, 0);
		SDL_FreeSurface(_surface);
		_surface = surface;
	}

	_width = _surface->w;
	_height = _surface->h;
	_pixels = (char*)_surface->pixels;
	_owner = false;

	PremultiplyAlpha();

#endif
}


Image::~Image()
{
#ifdef OPENWAR_IMAGE_USE_SDL
	SDL_FreeSurface(_surface);
#endif

#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
	CGContextRelease(_context);
#endif

	if (_owner)
		free(_pixels);
}


#ifdef OPENWAR_IMAGE_USE_SDL
SDL_Surface* Image::GetSurface()
{
	return _surface;
}
#endif


#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
void Image::LoadFromCGImage(CGImageRef image)
{
	_width = CGImageGetWidth(image);
	_height = CGImageGetHeight(image);
	_pixels = (unsigned char*)calloc((size_t)(_width * _height * 4), sizeof(unsigned char));
	_owner = true;

	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	_context = CGBitmapContextCreate(_pixels, (size_t)_width, (size_t)_height, 8, (size_t)_width * 4, colorSpace, kCGImageAlphaPremultipliedLast);
	CGColorSpaceRelease(colorSpace);

	CGContextDrawImage(_context, CGRectMake(0.0f, 0.0f, (CGFloat)_width, (CGFloat)_height), image);
}
#endif



#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
CGContextRef Image::GetCGContext() const
{
	return _context;
}
#endif


int Image::GetWidth() const
{
	return _width;
}


int Image::GetHeight() const
{
	return _height;
}


const void* Image::GetPixels() const
{
	return _pixels;
}


glm::vec4 Image::GetPixel(int x, int y) const
{
	if (0 <= x && x < _width && 0 <= y && y < _height)
	{
		const unsigned char* p = _pixels + 4 * (x + _width * y);
		return glm::vec4(p[0], p[1], p[2], p[3]) / 255.0f;
	}
	return glm::vec4();
}


void Image::SetPixel(int x, int y, glm::vec4 c)
{
	if (0 <= x && x < _width && 0 <= y && y < _height)
	{
		bounds1f bounds(0, 255);
		unsigned char* p = _pixels + 4 * (x + _width * y);
		p[0] = (unsigned char)glm::round(bounds.clamp(c.r * 255));
		p[1] = (unsigned char)glm::round(bounds.clamp(c.g * 255));
		p[2] = (unsigned char)glm::round(bounds.clamp(c.b * 255));
		p[3] = (unsigned char)glm::round(bounds.clamp(c.a * 255));
	}
}


void Image::PremultiplyAlpha()
{
	for (int y = 0; y < _height; ++y)
		for (int x = 0; x < _width; ++x)
			{
				glm::vec4 c = GetPixel(x, y);
				c.r *= c.a;
				c.g *= c.a;
				c.b *= c.a;
				SetPixel(x, y, c);
			}
}



#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
NSData* ConvertImageToTiff(Image* map)
{
#if TARGET_OS_IPHONE
	return nil;
#else
	unsigned char* pixels = reinterpret_cast<unsigned char*>(const_cast<GLvoid*>(map->GetPixels()));
	NSBitmapImageRep* imageRep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:&pixels
														   pixelsWide:map->GetWidth()
														   pixelsHigh:map->GetHeight()
														   bitsPerSample:8
														   samplesPerPixel:4
														   hasAlpha:YES
														   isPlanar:NO
														   colorSpaceName:NSDeviceRGBColorSpace
														   bytesPerRow:4 * map->GetWidth()
														   bitsPerPixel:32];
	NSData* result = [imageRep TIFFRepresentationUsingCompression:NSTIFFCompressionLZW factor:0.5];
	[imageRep release];
	return result;
#endif
}
#endif


#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
Image* ConvertTiffToImage(NSData* data)
{
#if TARGET_OS_IPHONE
	Image* result = new Image();
	result->LoadFromCGImage([[UIImage imageWithData:data] CGImage]);
	return result;
#else
	NSImage* img = [[NSImage alloc] initWithData:data];
	NSSize size = img.size;
	NSRect rect = NSMakeRect(0, 0, size.width, size.height);
	Image* result = new Image();
	result->LoadFromCGImage([img CGImageForProposedRect:&rect context:nil hints:nil]);
	[img release];
	return result;
#endif
}
#endif
