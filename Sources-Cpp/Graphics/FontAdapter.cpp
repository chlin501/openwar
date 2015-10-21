// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "FontAdapter.h"
#include "GraphicsContext.h"
#include "Image.h"
#include "TextureAtlas.h"


#ifdef OPENWAR_USE_FONTADAPTER_NSFONT
#import <AppKit/AppKit.h>
#endif

#ifdef OPENWAR_USE_FONTADAPTER_UIFONT
#import <UIKit/UIKit.h>
#endif

#include <locale>
//#include <codecvt>


FontAdapter::~FontAdapter()
{
}


/* FontAdapter_NSFont */


#ifdef OPENWAR_USE_FONTADAPTER_NSFONT


FontAdapter_NSFont::FontAdapter_NSFont(GraphicsContext* gc, const FontDescriptor& fontDescriptor) :
	_font(nil),
	_attributes(nil)
{
	float size = fontDescriptor.size * gc->GetCombinedScaling();
	if (size == 0)
		size = 12 * gc->GetCombinedScaling();

	if (!fontDescriptor.name.empty())
		_font = [NSFont fontWithName:[NSString stringWithUTF8String:fontDescriptor.name.c_str()] size:size];
	else if (fontDescriptor.bold)
		_font = [NSFont boldSystemFontOfSize:size];
	else
		_font = [NSFont systemFontOfSize:size];

	_attributes = @{
		NSFontAttributeName: _font,
		NSForegroundColorAttributeName: [NSColor whiteColor]
	};
}


FontAdapter_NSFont::~FontAdapter_NSFont()
{
}


std::shared_ptr<TextureImage> FontAdapter_NSFont::AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter)
{
	NSString* string = [NSString stringWithUTF8String:character.c_str()];

	CGSize size = [string sizeWithAttributes:_attributes];

	float scaling = textureAtlas->GetGraphicsContext()->GetCombinedScaling();
	int offset = 1 + (int)glm::ceil(scaling * border);
	int width = (int)glm::ceil(size.width) + offset * 2;
	int height = (int)glm::ceil(size.height) + offset * 2;

	Image image(width, height);
	image.SetPixelDensity(textureAtlas->GetGraphicsContext()->GetCombinedScaling());

	CGContextRef context = image.GetCGContext();
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:[NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:NO]];

	CGContextSetRGBFillColor(context, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor(context, 1, 1, 1, 1);

	CGPoint p = CGPointMake(offset, offset);
	[string drawAtPoint:p withAttributes:_attributes];

	[NSGraphicsContext restoreGraphicsState];

	if (filter)
		filter(image);

	std::shared_ptr<TextureImage> textureImage = textureAtlas->AddTextureImage(image, TextureDiscardability::Discardable);

	BorderBounds bounds = textureImage->GetBounds();
	bounds.outer = bounds.outer.add_radius(-1);
	bounds.inner.min = bounds.outer.min + glm::vec2(offset, offset);
	bounds.inner.max = bounds.inner.min + glm::vec2(size.width, size.height);
	textureImage->SetBounds(bounds);

	return textureImage;
}


#endif


/* FontAdapter_UIFont */


#ifdef OPENWAR_USE_FONTADAPTER_UIFONT


FontAdapter_UIFont::FontAdapter_UIFont(GraphicsContext* gc, const FontDescriptor& fontDescriptor) :
	_font(nil),
	_attributes(nil)
{
	float size = fontDescriptor.size * gc->GetCombinedScaling();
	if (size == 0)
		size = 12 * gc->GetCombinedScaling();

	if (!fontDescriptor.name.empty())
		_font = [UIFont fontWithName:[NSString stringWithUTF8String:fontDescriptor.name.c_str()] size:size];
	else if (fontDescriptor.bold)
		_font = [UIFont boldSystemFontOfSize:size];
	else
		_font = [UIFont systemFontOfSize:size];

	_attributes = @{
		NSFontAttributeName: _font,
		NSForegroundColorAttributeName: [UIColor whiteColor]
	};
}


FontAdapter_UIFont::~FontAdapter_UIFont()
{
}


std::shared_ptr<TextureImage> FontAdapter_UIFont::AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter)
{
	NSString* string = [NSString stringWithUTF8String:character.c_str()];

	CGSize size = [string sizeWithAttributes:_attributes];

	float scaling = textureAtlas->GetGraphicsContext()->GetCombinedScaling();
	int offset = 1 + (int)glm::ceil(scaling * border);
	int width = (int)glm::ceil(size.width) + offset * 2;
	int height = (int)glm::ceil(size.height) + offset * 2;

	Image image(width, height);
	image.SetPixelDensity(scaling);

	CGContextRef context = image.GetCGContext();

	UIGraphicsPushContext(context);

	CGContextTranslateCTM(context, 0, height);
	CGContextScaleCTM(context, 1, -1);

	CGContextSetRGBFillColor(context, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor(context, 1, 1, 1, 1);

	CGPoint p = CGPointMake(offset, offset);

	[string drawAtPoint:p withAttributes:_attributes];

	UIGraphicsPopContext();

	if (filter)
		filter(image);

	std::shared_ptr<TextureImage> textureImage = textureAtlas->AddTextureImage(image, TextureDiscardability::Discardable);

	BorderBounds bounds = textureImage->GetBounds();
	bounds.outer = bounds.outer.add_radius(-1);
	bounds.inner.min = bounds.outer.min + glm::vec2(offset, offset);
	bounds.inner.max = bounds.inner.min + glm::vec2(size.width, size.height);
	textureImage->SetBounds(bounds);

	return textureImage;
}


#endif


/* FontAdapter_NSFont */


#ifdef OPENWAR_USE_FONTADAPTER_SDLTTF

#ifdef OPENWAR_PLATFORM_ANDROID
#define FONTNAME_REGULAR "/system/fonts/Roboto-Regular.ttf"
#define FONTNAME_BOLD "/system/fonts/Roboto-Bold.ttf"
#define FONTNAME_FALLBACK "/system/fonts/DroidSansFallback.ttf"
#define FONTNAME_EMOJI "/system/fonts/AndroidEmoji.ttf"
#else
#define FONTNAME_REGULAR "Roboto-Regular.ttf"
#define FONTNAME_BOLD "Roboto-Regular.ttf"
#define FONTNAME_FALLBACK "Roboto-Regular.ttf"
#define FONTNAME_EMOJI "Roboto-Regular.ttf"
#endif

FontAdapter_SDL_ttf::FontAdapter_SDL_ttf(GraphicsContext* gc, const FontDescriptor& fontDescriptor)
{
	const float scaling = 1.2f;

	if (!TTF_WasInit())
		TTF_Init();

	int size = (int)(scaling * fontDescriptor.size * gc->GetCombinedScaling());
	if (size == 0)
		size = (int)(scaling * 12 * gc->GetCombinedScaling());

	_font1 = TTF_OpenFont(fontDescriptor.bold ? FONTNAME_BOLD : FONTNAME_REGULAR, size);
	_font2 = TTF_OpenFont(FONTNAME_FALLBACK, size);
	//_emoji = TTF_OpenFont(FONTNAME_EMOJI, size);

	int style = fontDescriptor.bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL;
	int hinting = TTF_HINTING_NORMAL;

	if (_font1 != NULL)
	{
		//TTF_SetFontStyle(_font1, style);
		TTF_SetFontOutline(_font1, 0);
		TTF_SetFontKerning(_font1, 1);
		TTF_SetFontHinting(_font1, hinting);
	}

	if (_font2 != NULL)
	{
		//TTF_SetFontStyle(_font2, style);
		TTF_SetFontOutline(_font2, 0);
		TTF_SetFontKerning(_font2, 1);
		TTF_SetFontHinting(_font2, hinting);
	}

	if (_emoji != NULL)
	{
		//TTF_SetFontStyle(_emoji, style);
		TTF_SetFontOutline(_emoji, 0);
		TTF_SetFontKerning(_emoji, 1);
		TTF_SetFontHinting(_emoji, hinting);
	}
}


FontAdapter_SDL_ttf::~FontAdapter_SDL_ttf()
{
	if (_font1)
		TTF_CloseFont(_font1);

	if (_font2)
		TTF_CloseFont(_font2);

	if (_emoji)
		TTF_CloseFont(_emoji);

}


std::shared_ptr<TextureImage> FontAdapter_SDL_ttf::AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter)
{
	SDL_Surface* surface = nullptr;

	TTF_Font* font = FindFontForCharacter(character);
	if (font)
	{
		SDL_Color color = {255, 255, 255, 255};
		surface = TTF_RenderUTF8_Blended(font, character.c_str(), color);
	}

	if (!surface)
	{
		return textureAtlas->AddTextureImage(Image(4, 4), TextureDiscardability::Discardable);
	}

	Image image;
	image.LoadFromSurface(surface);

	float scaling = textureAtlas->GetGraphicsContext()->GetCombinedScaling();
	int offset = 1 + (int)glm::ceil(scaling * border);

	Image image2(image.GetWidth() + offset * 2, image.GetHeight() + offset * 2);
	image2.Copy(image, offset, offset);

	SDL_FreeSurface(surface);

  	image2.SetPixelDensity(scaling);

	if (filter)
		filter(image2);

	std::shared_ptr<TextureImage> textureImage = textureAtlas->AddTextureImage(image2, TextureDiscardability::Discardable);
	BorderBounds bounds = textureImage->GetBounds();
	bounds.outer = bounds.outer.add_radius(-1);
	bounds.inner.min = bounds.outer.min + glm::vec2(offset, offset);
	bounds.inner.max = bounds.inner.min + glm::vec2(image.GetWidth(), image.GetHeight());
	textureImage->SetBounds(bounds);

	return textureImage;
}


TTF_Font* FontAdapter_SDL_ttf::FindFontForCharacter(const std::string& character) const
{
	return _font1;
	/*
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv(".", L".");
	auto result = conv.from_bytes(character);
	if (!result.empty())
	{
		if (_emoji && TTF_GlyphIsProvided(_emoji, result[0]))
			return _emoji;

		if (_font1 && TTF_GlyphIsProvided(_font1, result[0]))
			return _font1;
	}

	return _font2;
	*/
}


#endif
