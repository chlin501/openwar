#ifndef TextureFont_H
#define TextureFont_H

class TextureAtlas;
class TextureChar;
class TextureImage;

#include <map>
#include "bounds.h"

#define OPENWAR_USE_NSFONT

#ifdef OPENWAR_USE_SDL
#include <SDL2_ttf/SDL_ttf.h>
#endif
#ifdef OPENWAR_USE_UIFONT
#import <UIKit/UIKit.h>
#endif
#ifdef OPENWAR_USE_NSFONT
#import <AppKit/AppKit.h>
#endif


class TextureFont
{
	TextureAtlas* _textureAtlas;
	NSFont* _font;
	NSDictionary* _attributes;
	std::map<std::string, TextureChar*> _textureChars;

public:
	TextureFont(TextureAtlas* textureAtlas, const char* name, float size);
	TextureFont(TextureAtlas* textureAtlas, bool bold, float size);
	~TextureFont();

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
