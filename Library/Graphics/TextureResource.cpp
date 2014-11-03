#include "TextureResource.h"
#include "Image.h"



TextureResource::TextureResource(GraphicsContext* gc, const resource& r) : Texture(gc)
{
	LoadTextureFromResource(gc, r);
}




#if TARGET_OS_IPHONE
static bool CheckForExtension(NSString *searchName)
{
    NSString *extensionsString = [NSString stringWithCString:(const char*)glGetString(GL_EXTENSIONS) encoding:NSASCIIStringEncoding];
    NSArray *extensionsNames = [extensionsString componentsSeparatedByString:@" "];
    return [extensionsNames containsObject: searchName];
}
#endif



void TextureResource::LoadTextureFromResource(GraphicsContext* gc, const resource& r)
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

