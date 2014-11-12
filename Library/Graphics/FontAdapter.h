#ifndef FontAdapter_H
#define FontAdapter_H

#include <functional>

#if TARGET_OS_IPHONE
#define OPENWAR_USE_UIFONT
#else
#define OPENWAR_USE_NSFONT
#endif

#ifdef OPENWAR_USE_SDL
#include <SDL2_ttf/SDL_ttf.h>
#endif
#ifdef OPENWAR_USE_UIFONT
#import <UIKit/UIKit.h>
#endif

class GraphicsContext;
class Image;
class TextureAtlas;
class TextureImage;


struct FontDescriptor
{
	std::string name;
	float size;
	bool bold;

	FontDescriptor() : name(), size(0), bold(false) { }
	FontDescriptor(const char* n, float s) : name(n), size(s), bold(false) { }
	FontDescriptor(bool b, float s) : name(), size(s), bold(b) { }
};


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
	virtual TextureImage* AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter) = 0;
};


#ifdef OPENWAR_USE_NSFONT
@class NSFont;
@class NSDictionary;
class FontAdapter_NSFont : public FontAdapter
{
	NSFont* _font;
	NSDictionary* _attributes;

public:
	FontAdapter_NSFont(GraphicsContext* gc, const FontDescriptor& fontDescriptor);
	virtual ~FontAdapter_NSFont();
	virtual TextureImage* AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, int border, std::function<void(Image&)> filter);
};
#endif


#endif
