// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TextureFont.h"
#include "Image.h"
#include "TextureAtlas.h"
#include "GraphicsContext.h"
#include "FontAdapter.h"

//#include <codecvt>
#include <cstdlib>
#include <locale>


TextureCharIterator::TextureCharIterator(const char* s) :
	_s{s}
{
	if (ContainsArabic(s))
		_c.assign(s);
	else
		_c.assign(s, Utf8CharSize(s));
}


void TextureCharIterator::MoveNext()
{
	_s += _c.size();
	_c.assign(_s, Utf8CharSize(_s));
}


wchar_t TextureCharIterator::Utf8CharCode(const char* s, std::size_t n)
{
	switch (n)
	{
		case 1: return s[0];
		case 0: return 0;
		case 2: return ((s[0] & 0x1f) <<  6) |  (s[1] & 0x3f);
		case 3: return ((s[0] & 0x0f) << 12) | ((s[1] & 0x3f) <<  6) |  (s[2] & 0x3f);
		case 4: return ((s[0] & 0x07) << 18) | ((s[1] & 0x3f) << 12) | ((s[2] & 0x3f) <<  6) |  (s[3] & 0x3f);
		case 5: return ((s[0] & 0x03) << 24) | ((s[1] & 0x3f) << 18) | ((s[2] & 0x3f) << 12) | ((s[3] & 0x3f) <<  6) |  (s[4] & 0x3f);
		case 6: return ((s[0] & 0x01) << 30) | ((s[1] & 0x3f) << 24) | ((s[2] & 0x3f) << 18) | ((s[3] & 0x3f) << 12) | ((s[4] & 0x3f) << 6) | (s[5] & 0x3f);
		default: return 0;
	}
}


std::size_t TextureCharIterator::Utf8CharSize(const char* s)
{
	if (char c = *s)
	{
		if ((c & 0x80) == 0) return 1;
		if ((c & 0x20) == 0) return 2;
		if ((c & 0x10) == 0) return 3;
		if ((c & 0x08) == 0) return 4;
		if ((c & 0x04) == 0) return 5;
		if ((c & 0x02) == 0) return 6;
	}
	return 0;
}


bool TextureCharIterator::IsArabic(wchar_t c)
{
	unsigned int x = static_cast<unsigned int>(c) & 0xFFFF;
	return (0x0600 <= x && x <= 0x08FF)
		|| (0xFB00 <= x && x <= 0xFDFF);
}


bool TextureCharIterator::ContainsArabic(const char* s)
{
	while (*s != '\0')
	{
		std::size_t n = Utf8CharSize(s);
		if (IsArabic(Utf8CharCode(s, n)))
			return true;
		s += n;
	}
	return false;
}


/***/


TextureFont::CharacterKey::CharacterKey(const char* s, float r)
{
	std::size_t n = TextureCharIterator::Utf8CharSize(s);
	_c = TextureCharIterator::Utf8CharCode(s, n);
	_r = r;
	if (n != 0)
		_s = s + n;
}


bool TextureFont::CharacterKey::operator<(const CharacterKey& other) const
{
	if (_c != other._c)
		return _c < other._c;
	if (_r != other._r)
		return _r < other._r;
	return _s < other._s;
}


TextureFont::TextureFont(TextureAtlas* textureAtlas, FontAdapter* fontAdapter) :
	_textureAtlas(textureAtlas),
	_fontAdapter(fontAdapter)
{
}


TextureFont::~TextureFont()
{
	for (auto i : _textureChars)
		delete i.second;
}


TextureChar* TextureFont::GetTextureChar(const std::string& character, float blurRadius)
{
	CharacterKey key(character.c_str(), blurRadius);

	TextureChar* textureChar = _textureChars[key];
	if (textureChar && !textureChar->GetTextureImage()->IsDiscarded())
		return textureChar;

	bool canColorize = false;
	std::function<void(Image&)> filter = [&canColorize, blurRadius](Image& image) {
		canColorize = image.IsGrayscale();
		if (blurRadius != 0)
		{
			image.ApplyBlurFilter(blurRadius);
			image.NormalizeAlpha();
		}
		image.PremultiplyAlpha();
	};

	int border = 4 + (int)glm::ceil(2 * blurRadius);

	if (!_fontAdapter)
		return nullptr;

	std::shared_ptr<TextureImage> textureImage = _fontAdapter->AddTextureImage(_textureAtlas, character, border, filter);
	if (!textureImage)
		return nullptr;

	TextureChar* result = new TextureChar(textureImage, canColorize);
	_textureChars[key] = result;

	delete textureChar;

	return result;
}


glm::vec2 TextureFont::MeasureText(const char* text)
{
	float w = 0;
	float h = 0;

	TextureCharIterator chars(text);

	while (!chars.GetChar().empty())
	{
		TextureChar* item = GetTextureChar(chars.GetChar(), 0);
		glm::vec2 size = item->GetInnerSize();
		w += size.x;
		h = fmaxf(h, size.y);
		chars.MoveNext();
	}

	return glm::vec2(w, h);
}


/***/


TextureChar::TextureChar(std::shared_ptr<TextureImage> textureImage, bool canColorize) :
	_textureImage(textureImage),
	_canColorize(canColorize)
{
}


std::shared_ptr<TextureImage> TextureChar::GetTextureImage() const
{
	return _textureImage;
}


bool TextureChar::CanColorize() const
{
	return _canColorize;
}


glm::vec2 TextureChar::GetInnerSize() const
{
	return _textureImage->GetBounds().inner.size() / _textureImage->GetDensity();
}


bounds2f TextureChar::GetOuterUV() const
{
	return _textureImage->GetCoords().outer;
}


bounds2f TextureChar::GetOuterXY(glm::vec2 position) const
{
	BorderBounds bounds = _textureImage->GetBounds();
	float density = _textureImage->GetDensity();
	return (bounds.outer - bounds.inner.min) / density + position;
}
