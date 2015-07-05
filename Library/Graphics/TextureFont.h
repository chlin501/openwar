// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TextureFont_H
#define TextureFont_H

#include <map>
#include <string>
#include "Algebra/bounds.h"

class GraphicsContext;
class FontAdapter;
class TextureAtlas;
class TextureChar;
class TextureImage;


class TextureCharIterator
{
	//static_assert(sizeof(wchar_t) >= 4);
	const char* _s;
	std::string _c;

public:
	TextureCharIterator(const char* s);

	const std::string& GetChar() const { return _c; }
	void MoveNext();

	static wchar_t Utf8CharCode(const char* s, std::size_t n);
	static std::size_t Utf8CharSize(const char* s);

	static bool IsArabic(wchar_t c);
	static bool ContainsArabic(const char* s);
};


class TextureFont
{
	class CharacterKey
	{
		int _c;
		float _r;
		std::string _s;
	public:
		CharacterKey(const char* s, float r);
		bool operator<(const CharacterKey& other) const;
	};

	TextureAtlas* _textureAtlas;
	FontAdapter* _fontAdapter;
	std::map<CharacterKey, TextureChar*> _textureChars;

public:
	TextureFont(TextureAtlas* textureAtlas, FontAdapter* fontAdapter);
	~TextureFont();

	TextureFont(const TextureFont&) = delete;
	TextureFont& operator=(const TextureFont&) = delete;

	TextureChar* GetTextureChar(const std::string& character, float blurRadius);
	glm::vec2 MeasureText(const char* text);
};


class TextureChar
{
	std::shared_ptr<TextureImage> _textureImage;
	bool _canColorize;

public:
	TextureChar(std::shared_ptr<TextureImage> textureImage, bool canColorize);

	std::shared_ptr<TextureImage> GetTextureImage() const;
	bool CanColorize() const;

	glm::vec2 GetInnerSize() const;
	bounds2f GetOuterUV() const;
	bounds2f GetOuterXY(glm::vec2 position) const;

};


#endif
