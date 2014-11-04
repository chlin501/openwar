#ifndef TextureFont_H
#define TextureFont_H

class TextureAtlas;
class TextureChar;
class TextureImage;

#import <AppKit/AppKit.h>
#import "bounds.h"

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

public:
	TextureFont(TextureAtlas* textureAtlas, NSFont* font);
	~TextureFont();

	TextureChar* GetTextureChar(const std::string& character);
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
