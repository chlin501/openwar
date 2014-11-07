#ifndef TextureFont_H
#define TextureFont_H

#include <map>
#include <string>
#include "bounds.h"

class GraphicsContext;
class TextureAtlas;
class TextureChar;
class TextureImage;

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
#ifdef OPENWAR_USE_NSFONT
#import <AppKit/AppKit.h>
#endif


struct FontDescriptor
{
	std::string name;
	float size;
	bool bold;

	FontDescriptor() : name(), size(0), bold(false) { }
	FontDescriptor(const char* n, float s) : name(n), size(s), bold(false) { }
	FontDescriptor(bool b, float s) : name(), size(s), bold(b) { }

	bool operator==(const FontDescriptor& other) const
	{
		return name == other.name
			&& size == other.size
			&& bold == other.bold;
	}
};



class TextureFont
{
	TextureAtlas* _textureAtlas;
	FontDescriptor _fontDescriptor;

	NSFont* _font;
	NSDictionary* _attributes;
	std::map<std::string, TextureChar*> _textureChars;

public:
	TextureFont(TextureAtlas* textureAtlas, const FontDescriptor& fontDescriptor);
	~TextureFont();

	const FontDescriptor& GetFontDescriptor();

	TextureChar* GetTextureChar(const std::string& character);
	glm::vec2 MeasureText(const char* text);
};


class TextureChar
{
	TextureImage* _textureImage;

public:
	TextureChar(TextureImage* textureImage);

	glm::vec2 GetInnerSize() const;
	bounds2f GetOuterUV() const;
	bounds2f GetOuterXY(glm::vec2 position) const;

	TextureImage* GetTextureImage() const;
};


#endif
