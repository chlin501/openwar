// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TextureResource.h"
#include "GraphicsContext.h"
#include "Image.h"

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif


TextureResource::TextureResource(GraphicsContext* gc, const Resource& r) : Texture(gc)
{
	LoadTextureFromResource(gc, r);
}




static bool HasSupportForTextureCompressionPvrtc()
{
	static bool initialized = false;
	static bool result = false;
	if (!initialized)
	{
#if TARGET_OS_IPHONE
		NSString* extensionsString = [NSString stringWithCString:(const char*)glGetString(GL_EXTENSIONS) encoding:NSASCIIStringEncoding];
		NSArray* extensionsNames = [extensionsString componentsSeparatedByString:@" "];
		result = [extensionsNames containsObject:@"GL_IMG_texture_compression_pvrtc"];
#endif
		initialized = true;
	}
	return result;
}



void TextureResource::LoadTextureFromResource(GraphicsContext* gc, const Resource& r)
{
#if TARGET_OS_IPHONE

	if (HasSupportForTextureCompressionPvrtc())
	{
		NSString* name = [NSString stringWithUTF8String:r.name()];
		NSString* path = [[NSBundle mainBundle] pathForResource:name ofType:@"pvrtc" inDirectory:@""];
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

#endif

	Image image;
	image.LoadFromResource(r);

	LoadTextureFromImage(image);
	GenerateMipmap();

}


void TextureResource::UpdateTexture()
{
}
