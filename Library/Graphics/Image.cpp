// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Image.h"
#include "bounds.h"
#include "GraphicsContext.h"
#include "Algorithms/GaussBlur.h"

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
	_context(NULL),
	_image(NULL),
#endif
	_width(0),
	_height(0),
	_pixels(nullptr),
	_owner(false)
{
}


Image::Image(const Image& image) :
#ifdef OPENWAR_IMAGE_USE_SDL
	_surface(nullptr),
#endif
#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
	_context(NULL),
	_image(NULL),
#endif
	_width(image._width),
	_height(image._height),
	_pixels(nullptr),
	_owner(false)
{
	_pixels = (unsigned char*) calloc((size_t)(_width * _height), 4);
	std::memcpy(_pixels, image._pixels, (size_t)(_width * _height) * 4);
	_owner = true;
}


Image::Image(int width, int height) :
#ifdef OPENWAR_IMAGE_USE_SDL
	_surface(nullptr),
#endif
#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
	_context(NULL),
	_image(NULL),
#endif
	_width(width),
	_height(height),
	_pixels(nullptr),
	_owner(false)
{
	_pixels = (unsigned char*) calloc((size_t)(_width * _height), 4);
	_owner = true;
}


Image::~Image()
{
#ifdef OPENWAR_IMAGE_USE_SDL

	if (_surface != nullptr)
		SDL_FreeSurface(_surface);

#endif

#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS

	if (_context != NULL)
		CGContextRelease(_context);

	if (_image != NULL)
		CGImageRelease(_image);

#endif

	if (_owner)
		free(_pixels);
}


Image& Image::LoadFromResource(const resource& r)
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

#endif

	return *this;

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
	_pixels = (unsigned char*)_surface->pixels;
	_owner = false;

	PremultiplyAlpha();

	return *this;

#endif
}


#ifdef OPENWAR_IMAGE_USE_SDL
SDL_Surface* Image::GetSurface() const
{
	if (_surface == nullptr)
	{
		_surface = SDL_CreateRGBSurface(0, _width, _height, 32, 0, 0, 0, 0);
	}

	return _surface;
}
#endif


#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
void Image::LoadFromCGImage(CGImageRef image)
{
	_width = CGImageGetWidth(image);
	_height = CGImageGetHeight(image);
	_pixels = (unsigned char*)calloc((size_t)(_width * _height), 4);
	_owner = true;

	CGRect rect = CGRectMake(0.0f, 0.0f, (CGFloat)_width, (CGFloat)_height);

	CGContextDrawImage(GetCGContext(), rect, image);
}
#endif



#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
CGContextRef Image::GetCGContext() const
{
	if (_context == NULL)
	{
		CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

		size_t width = (size_t)_width;
		size_t height = (size_t)_height;
		size_t bitsPerComponent = 8;
		size_t bytesPerRow = width * 4;
		_context = CGBitmapContextCreate(_pixels, width, height, bitsPerComponent, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast);

		CGColorSpaceRelease(colorSpace);
	}
	return _context;
}
#endif


#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
CGImageRef Image::GetCGImage() const
{
	if (_image == NULL)
	{
		size_t width = (size_t)_width;
		size_t height = (size_t)_height;
		size_t bitsPerComponent = 8;
		size_t bitsPerPixel = bitsPerComponent * 4;
		size_t bytesPerRow = width * 4;

		CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
		CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, _pixels, bytesPerRow * height, NULL);

		_image = CGImageCreate(width, height, bitsPerComponent, bitsPerPixel, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast, dataProvider, NULL, false, kCGRenderingIntentDefault);

		CGDataProviderRelease(dataProvider);
		CGColorSpaceRelease(colorSpace);
	}

	return _image;
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


static int CalculateSaturation(const unsigned char* pixel)
{
	int r = pixel[0];
	int g = pixel[1];
	int b = pixel[2];
	int M = glm::max(glm::max(r, g), b);
	int m = glm::min(glm::min(r, g), b);
	int C = M - m;
	int V = M;
	return V == 0 ? 0 : 255 * C / V;
}


bool Image::IsGrayscale() const
{
	const unsigned char* end = _pixels + _height * _width * 4;
	for (const unsigned char* pixel = _pixels; pixel != end; pixel += 4)
		if (pixel[3] > 32 && CalculateSaturation(pixel) > 16)
			return false;

	return true;
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


void Image::Blur(float r)
{
	int w = GetWidth();
	int h = GetHeight();
	glm::vec4* scl = new glm::vec4[w * h];
	glm::vec4* tcl = new glm::vec4[w * h];

	int i = 0;
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x)
		{
			scl[i] = tcl[i] = GetPixel(x, y);
			++i;
		}

	GaussBlur(scl, tcl, w, h, r);

	i = 0;
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x)
			SetPixel(x, y, tcl[i++]);

	delete [] scl;
	delete [] tcl;

}




void Image::Copy(const Image& image, int x, int y)
{
#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
	CGContextRef context = GetCGContext();
	int width = image.GetWidth();
	int height = image.GetHeight();
	CGRect rect = CGRectMake(x, _height - y - height, width, height);
	CGContextClearRect(context, rect);
	CGContextDrawImage(context, rect, image.GetCGImage());
#endif
}


void Image::Fill(const glm::vec4& color, const bounds2f& bounds)
{
#ifdef OPENWAR_IMAGE_USE_COREGRAPHICS
	CGContextRef context = GetCGContext();

	//NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:YES];
	//[NSGraphicsContext saveGraphicsState];
	//[NSGraphicsContext setCurrentContext:gc];

	CGContextSetRGBFillColor(context, color.r, color.g, color.b, color.a);
	CGRect rect = CGRectMake(bounds.min.x, bounds.min.y, bounds.x().size(), bounds.y().size());
	CGContextClearRect(context, rect);
	CGContextFillRect(context, rect);

	//[NSGraphicsContext restoreGraphicsState];
#endif
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
