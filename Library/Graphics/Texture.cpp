// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
#import <Foundation/Foundation.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif
#endif

#include "Texture.h"
#include "ShaderProgram.h"
#include "Image.h"
#import "GraphicsContext.h"



Texture::Texture(GraphicsContext* gc)
{
	glGenTextures(1, &id);
	CHECK_ERROR_GL();
	init();
}


Texture::~Texture()
{
	glDeleteTextures(1, &id);
	CHECK_ERROR_GL();
}


void Texture::init()
{
	glBindTexture(GL_TEXTURE_2D, id);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_CLAMP_TO_EDGE
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_CLAMP_TO_EDGE
	CHECK_ERROR_GL();
}


/***/


TextureXXX::TextureXXX(GraphicsContext* gc, const Image& image) : Texture(gc)
{
	load(image);
}


TextureXXX::TextureXXX(GraphicsContext* gc, SDL_Surface* surface) : Texture(gc)
{
	load(surface);
}


void TextureXXX::load(const Image& image)
{
	glBindTexture(GL_TEXTURE_2D, id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, image.format(), GL_UNSIGNED_BYTE, image.pixels());
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();
}



void TextureXXX::load(SDL_Surface* surface)
{
	SDL_Surface* tmp = nullptr;
	if (surface->format->format != SDL_PIXELFORMAT_ABGR8888)
	{
		tmp = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
		surface = tmp;
	}

	SDL_LockSurface(surface);

	glBindTexture(GL_TEXTURE_2D, id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();

	SDL_UnlockSurface(surface);

	if (tmp != nullptr)
		SDL_FreeSurface(tmp);
}


/***/


TextureResource::TextureResource(GraphicsContext* gc, const resource& r) : Texture(gc)
{
	load(gc, r);
}




#if TARGET_OS_IPHONE
static bool CheckForExtension(NSString *searchName)
{
    NSString *extensionsString = [NSString stringWithCString:(const char*)glGetString(GL_EXTENSIONS) encoding:NSASCIIStringEncoding];
    NSArray *extensionsNames = [extensionsString componentsSeparatedByString:@" "];
    return [extensionsNames containsObject: searchName];
}
#endif



void TextureResource::load(GraphicsContext* gc, const resource& r)
{
#ifdef OPENWAR_USE_SDL

	ImageResource img(gc, r);
	img.PremultiplyAlpha();
	load(img);

#else

#if TARGET_OS_IPHONE
	UIImage* image = nil;

	NSString* name = [NSString stringWithFormat:@"%@%@", [NSString stringWithUTF8String:r.name()], [NSString stringWithUTF8String:r.type()]];
	if ([name hasSuffix:@".png"])
	{
		NSString* stem = [name substringToIndex:name.length - 4];

		if (CheckForExtension(@"GL_IMG_texture_compression_pvrtc"))
		{
			NSString* path = [[NSBundle mainBundle] pathForResource:stem ofType:@"pvrtc" inDirectory:@""];
			NSData* pvrtc = [NSData dataWithContentsOfFile:path];
			if (pvrtc != nil)
			{
				glBindTexture(GL_TEXTURE_2D, id);
				CHECK_ERROR_GL();
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, 1024, 1024, 0, pvrtc.length, pvrtc.bytes);
				CHECK_ERROR_GL();
				return;
			}
		}

		if (gc->GetPixelDensity() > 1)
		{
			NSString* name2x = [NSString stringWithFormat:@"%@@2x.png", stem];
			image = [UIImage imageNamed:name2x];
		}
	}

	if (image == nil)
		image = [UIImage imageNamed:name];

	load(ImageCG(image.CGImage));

#else

    ResourceImage img(gc, r);

	glBindTexture(GL_TEXTURE_2D, id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, img.format(), GL_UNSIGNED_BYTE, img.pixels());
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();

	/*
	NSImage* image = nil;
	if ([name hasSuffix:@".png"])
	{
		NSString* stem = [name substringToIndex:name.length - 4];
		image = [NSImage imageNamed:stem];
	}

	if (image == nil)
		image = [NSImage imageNamed:name];

	load(image::image([image CGImageForProposedRect:nil context:nil hints:nil]));
	 */

#endif

#endif
}

