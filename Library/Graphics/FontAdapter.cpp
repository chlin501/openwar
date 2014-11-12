#include "FontAdapter.h"
#include "GraphicsContext.h"
#include "Image.h"
#include "TextureAtlas.h"

#ifdef ENABLE_FONTADAPTER_NSFONT
#import <AppKit/AppKit.h>
#endif

#ifdef ENABLE_FONTADAPTER_UIFONT
#import <UIKit/UIKit.h>
#endif



FontAdapter::~FontAdapter()
{
}


/* FontAdapter_NSFont */


#ifdef ENABLE_FONTADAPTER_NSFONT


FontAdapter_NSFont::FontAdapter_NSFont(GraphicsContext* gc, const FontDescriptor& fontDescriptor) :
	_font(nil),
	_attributes(nil)
{
	float size = fontDescriptor.size * gc->GetPixelDensity();
	if (size == 0)
		size = 12 * gc->GetPixelDensity();

	if (!fontDescriptor.name.empty())
		_font = [[NSFont fontWithName:[NSString stringWithUTF8String:fontDescriptor.name.c_str()] size:size] retain];
	else if (fontDescriptor.bold)
		_font = [[NSFont boldSystemFontOfSize:size] retain];
	else
		_font = [[NSFont systemFontOfSize:size] retain];

	_attributes = [@{
		NSFontAttributeName: _font,
		NSForegroundColorAttributeName: [NSColor whiteColor]
	} retain];

}


FontAdapter_NSFont::~FontAdapter_NSFont()
{
	[_font release];
	[_attributes release];
}


std::shared_ptr<TextureImage> FontAdapter_NSFont::AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter)
{
	NSString* string = [NSString stringWithUTF8String:character.c_str()];

	CGSize size = [string sizeWithAttributes:_attributes];

	int width = (int)glm::ceil(size.width) + border * 2;
	int height = (int)glm::ceil(size.height) + border * 2;

	Image image(width, height);

	CGContextRef context = image.GetCGContext();

	NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:NO];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:gc];

	CGContextSetRGBFillColor(context, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor(context, 1, 1, 1, 1);

	CGPoint p = CGPointMake(border, border);
	[string drawAtPoint:p withAttributes:_attributes];

	[NSGraphicsContext restoreGraphicsState];

	if (filter)
		filter(image);

	std::shared_ptr<TextureImage> textureImage = textureAtlas->AddTextureImage(image, TextureImageType::Discardable);
	textureImage->_inner.min = textureImage->_outer.min + glm::vec2(border, border);
	textureImage->_inner.max = textureImage->_inner.min + glm::vec2(size.width, size.height);

	return textureImage;
}


#endif


/* FontAdapter_UIFont */


#ifdef ENABLE_FONTADAPTER_UIFONT


FontAdapter_UIFont::FontAdapter_UIFont(GraphicsContext* gc, const FontDescriptor& fontDescriptor) :
	_font(nil),
	_attributes(nil)
{
	float size = fontDescriptor.size * gc->GetPixelDensity();
	if (size == 0)
		size = 12 * gc->GetPixelDensity();

	if (!fontDescriptor.name.empty())
		_font = [[UIFont fontWithName:[NSString stringWithUTF8String:fontDescriptor.name.c_str()] size:size] retain];
	else if (fontDescriptor.bold)
		_font = [[UIFont boldSystemFontOfSize:size] retain];
	else
		_font = [[UIFont systemFontOfSize:size] retain];

	_attributes = [@{
//		NSFontAttributeName: _font,
//		NSForegroundColorAttributeName: [UIColor whiteColor]
	} retain];
}


FontAdapter_UIFont::~FontAdapter_UIFont()
{
	[_font release];
	[_attributes release];
}


std::shared_ptr<TextureImage> FontAdapter_UIFont::AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter)
{
	NSString* string = [NSString stringWithUTF8String:character.c_str()];

	CGSize size = [string sizeWithFont:_font];
	//CGSize size = [string sizeWithAttributes:_attributes];

	int width = (int)glm::ceil(size.width) + border * 2;
	int height = (int)glm::ceil(size.height) + border * 2;

	Image image(width, height);

	CGContextRef context = image.GetCGContext();

	UIGraphicsPushContext(context);

	CGContextTranslateCTM(context, 0, height);
	CGContextScaleCTM(context, 1, -1);

	CGContextSetRGBFillColor(context, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor(context, 1, 1, 1, 1);

	CGPoint p = CGPointMake(border, border);

	[string drawAtPoint:p withFont:_font];
	//[string drawAtPoint:p withAttributes:_attributes];

	UIGraphicsPopContext();

	if (filter)
		filter(image);

	std::shared_ptr<TextureImage> textureImage = textureAtlas->AddTextureImage(image, TextureImageType::Discardable);
	textureImage->_inner.min = textureImage->_outer.min + glm::vec2(border, border);
	textureImage->_inner.max = textureImage->_inner.min + glm::vec2(size.width, size.height);

	return textureImage;
}


#endif


/* FontAdapter_NSFont */


#ifdef ENABLE_FONTADAPTER_SDL_TTF

#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#define ANDROID_FONT1 "Roboto-Regular.ttf"
#define ANDROID_FONT2 "Roboto-Regular.ttf"
#define ANDROID_EMOJI "Roboto-Regular.ttf"
#else
#define ANDROID_FONT1 "/system/fonts/Roboto-Regular.ttf"
#define ANDROID_FONT2 "/system/fonts/DroidSansFallback.ttf"
#define ANDROID_EMOJI "/system/fonts/AndroidEmoji.ttf"
#endif

FontAdapter_SDL_ttf::FontAdapter_SDL_ttf(GraphicsContext* gc, const FontDescriptor& fontDescriptor)
{
	if (!TTF_WasInit())
		TTF_Init();

	int size = (int)fontDescriptor.size;
	if (size == 0)
		size = 12;

	_font1 = TTF_OpenFont(ANDROID_FONT1, size);
	_font2 = TTF_OpenFont(ANDROID_FONT2, size);
	_emoji = TTF_OpenFont(ANDROID_EMOJI, size);

	if (_font1 != NULL)
	{
		TTF_SetFontStyle(_font1, TTF_STYLE_NORMAL);
		TTF_SetFontOutline(_font1, 0);
		TTF_SetFontKerning(_font1, 1);
		TTF_SetFontHinting(_font1, TTF_HINTING_LIGHT);
	}

	if (_font2 != NULL)
	{
		TTF_SetFontStyle(_font2, TTF_STYLE_NORMAL);
		TTF_SetFontOutline(_font2, 0);
		TTF_SetFontKerning(_font2, 1);
		TTF_SetFontHinting(_font2, TTF_HINTING_LIGHT);
	}

	if (_emoji != NULL)
	{
		TTF_SetFontStyle(_emoji, TTF_STYLE_NORMAL);
		TTF_SetFontOutline(_emoji, 0);
		TTF_SetFontKerning(_emoji, 1);
		TTF_SetFontHinting(_emoji, TTF_HINTING_LIGHT);
	}
}


FontAdapter_SDL_ttf::~FontAdapter_SDL_ttf()
{
	if (_font1 != nullptr)
		TTF_CloseFont(_font1);

	if (_font2 != nullptr)
		TTF_CloseFont(_font2);

	if (_emoji != nullptr)
		TTF_CloseFont(_emoji);

}


std::shared_ptr<TextureImage> FontAdapter_SDL_ttf::AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter)
{
	SDL_Surface* surface = nullptr;

	if (_font1 != nullptr)
	{
		SDL_Color color = {255, 255, 255, 255};
		TTF_RenderUTF8_Blended(_font1, character.c_str(), color);
	}

	if (surface == nullptr)
	{
		return textureAtlas->AddTextureImage(Image(4, 4), TextureImageType::Discardable);
	}

	Image image;
	image.LoadFromSurface(surface);

	Image image2(image.GetWidth() + border * 2, image.GetHeight() + border * 2);
	image2.Copy(image, border, border);

	SDL_FreeSurface(surface);

	if (filter)
		filter(image2);

	std::shared_ptr<TextureImage> textureImage = textureAtlas->AddTextureImage(image2, TextureImageType::Discardable);
	textureImage->_inner.min = textureImage->_outer.min + glm::vec2(border, border);
	textureImage->_inner.max = textureImage->_inner.min + glm::vec2(image.GetWidth(), image.GetHeight());

	return textureImage;
}


#endif
