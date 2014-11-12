#ifndef TextureFont_H
#define TextureFont_H

#include <map>
#include <string>
#include "bounds.h"

class GraphicsContext;
class FontAdapter;
class TextureAtlas;
class TextureChar;
class TextureImage;


class TextureFont
{
	typedef std::pair<std::string, float> CharacterKey;
	TextureAtlas* _textureAtlas;
	FontAdapter* _fontAdapter;
	std::map<CharacterKey, TextureChar*> _textureChars;

public:
	TextureFont(TextureAtlas* textureAtlas, FontAdapter* fontAdapter);
	~TextureFont();

	TextureChar* GetTextureChar(const std::string& character, float blur);
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
