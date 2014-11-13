// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TextureResource.h"
#include "GraphicsContext.h"
#include "Image.h"

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif


TextureResource::TextureResource(GraphicsContext* gc, const resource& r) : Texture(gc)
{
	LoadTextureFromResource(gc, r);
}




#if TARGET_OS_IPHONE
static bool HasSupportForTextureCompressionPvrtc()
{
    NSString *extensionsString = [NSString stringWithCString:(const char*)glGetString(GL_EXTENSIONS) encoding:NSASCIIStringEncoding];
    NSArray *extensionsNames = [extensionsString componentsSeparatedByString:@" "];
    return [extensionsNames containsObject: @"GL_IMG_texture_compression_pvrtc"];
}
#endif



void TextureResource::LoadTextureFromResource(GraphicsContext* gc, const resource& r)
{
#if TARGET_OS_IPHONE

	UIImage* image = nil;

	NSString* name = [NSString stringWithFormat:@"%@%@", [NSString stringWithUTF8String:r.name()], [NSString stringWithUTF8String:r.type()]];
	if ([name hasSuffix:@".png"])
	{
		NSString* stem = [name substringToIndex:name.length - 4];

		if (HasSupportForTextureCompressionPvrtc())
		{
			NSString* path = [[NSBundle mainBundle] pathForResource:stem ofType:@"pvrtc" inDirectory:@""];
			NSData* pvrtc = [NSData dataWithContentsOfFile:path];
			if (pvrtc != nil)
			{
				glBindTexture(GL_TEXTURE_2D, _id);
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

	Image img;
	img.LoadFromCGImage(image.CGImage);

	glBindTexture(GL_TEXTURE_2D, _id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.GetWidth(), img.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.GetPixels());
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();

#else

	Image image;
	image.LoadFromResource(r);

	glBindTexture(GL_TEXTURE_2D, _id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixels());
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();

#endif
}


void TextureResource::UpdateTexture()
{
}
