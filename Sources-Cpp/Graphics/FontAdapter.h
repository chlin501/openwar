// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef FontAdapter_H
#define FontAdapter_H

#if defined(OPENWAR_PLATFORM_MAC)
#define OPENWAR_USE_FONTADAPTER_NSFONT
#elif defined(OPENWAR_PLATFORM_IOS)
#define OPENWAR_USE_FONTADAPTER_UIFONT
#elif defined(OPENWAR_USE_SDL) && !defined(OPENWAR_PLATFORM_WEB)
#define OPENWAR_USE_FONTADAPTER_SDLTTF
#endif


#include <functional>
#include <memory>

#include "FontDescriptor.h"

#ifdef OPENWAR_USE_FONTADAPTER_SDLTTF
#include <SDL2/SDL_ttf.h>
#endif

class GraphicsContext;
class Image;
class TextureAtlas;
class TextureImage;


inline bool operator==(const FontDescriptor& x, const FontDescriptor& y)
{
	return x.size == y.size
		&& x.bold == y.bold
		&& x.name == y.name;
}

inline bool operator<(const FontDescriptor& x, const FontDescriptor& y)
{
	if (x.size != y.size)
		return x.size < y.size;
	if (x.bold != y.bold)
		return x.bold < y.bold;
	return x.name < y.name;
}


class FontAdapter
{
public:
	virtual ~FontAdapter();
	virtual std::shared_ptr<TextureImage> AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter) = 0;
};


#ifdef OPENWAR_USE_FONTADAPTER_NSFONT
@class NSFont;
@class NSDictionary;
class FontAdapter_NSFont : public FontAdapter
{
	NSFont* _font;
	NSDictionary* _attributes;

public:
	FontAdapter_NSFont(GraphicsContext* gc, const FontDescriptor& fontDescriptor);
	virtual ~FontAdapter_NSFont();
	virtual std::shared_ptr<TextureImage> AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter);
};
#endif


#ifdef OPENWAR_USE_FONTADAPTER_UIFONT
@class UIFont;
@class NSDictionary;
class FontAdapter_UIFont : public FontAdapter
{
	UIFont* _font;
	NSDictionary* _attributes;

public:
	FontAdapter_UIFont(GraphicsContext* gc, const FontDescriptor& fontDescriptor);
	virtual ~FontAdapter_UIFont();
	virtual std::shared_ptr<TextureImage> AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter);
};
#endif


#ifdef OPENWAR_USE_FONTADAPTER_SDLTTF
class FontAdapter_SDL_ttf : public FontAdapter
{
	TTF_Font* _font1{};
	TTF_Font* _font2{};
	TTF_Font* _emoji{};

public:
	FontAdapter_SDL_ttf(GraphicsContext* gc, const FontDescriptor& fontDescriptor);
	virtual ~FontAdapter_SDL_ttf();
	virtual std::shared_ptr<TextureImage> AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter);

private:
	TTF_Font* FindFontForCharacter(const std::string& character) const;
};
#endif


#endif
