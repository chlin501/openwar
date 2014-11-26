// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TextureFont.h"
#include "Image.h"
#include "TextureAtlas.h"
#include "GraphicsContext.h"

#include <codecvt>
#include <cstdlib>
#include <locale>


static bool ContainsArabic(const std::wstring& ws) { return false; }



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
	CharacterKey key(character, blurRadius);

	auto i = _textureChars.find(key);
	if (i != _textureChars.end() && !i->second->GetTextureImage()->IsDiscarded())
		return i->second;

	bool canColorize = false;
	std::function<void(Image&)> filter = [&canColorize, blurRadius](Image& image) {
		canColorize = image.IsGrayscale();
		if (blurRadius != 0)
		{
			image.ApplyBlurFilter(glm::abs(blurRadius));
			if (blurRadius < 0)
			{
				image.ApplyPixelFilter([](glm::vec4 c) {
					float a = 1 - c.a;
					return glm::vec4(c.r, c.g, c.b, 1 - a * a);
				});
			}
		}
		image.PremultiplyAlpha();
	};

	int border = 2 + (int)glm::ceil(glm::abs(blurRadius));

	std::shared_ptr<TextureImage> textureImage = _fontAdapter->AddTextureImage(_textureAtlas, character, border, filter);
	if (textureImage == nullptr)
		return nullptr;

	TextureChar* textureChar = new TextureChar(textureImage, canColorize);
	_textureChars[key] = textureChar;
	return textureChar;
}


glm::vec2 TextureFont::MeasureText(const char* text)
{
	float w = 0;
	float h = 0;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv(".", L".");
	std::wstring ws = conv.from_bytes(text);

	if (ContainsArabic(ws))
	{
		TextureChar* item = GetTextureChar(text, 0);
		glm::vec2 size = item->GetInnerSize();
		w = size.x;
		h = size.y;
	}
	else
	{
		for (wchar_t wc : ws)
		{
			if (wc == 0)
				continue;

			std::string character = conv.to_bytes(&wc, &wc + 1);
			if (character.empty())
				continue;

			TextureChar* item = GetTextureChar(character, 0);
			glm::vec2 size = item->GetInnerSize();
			w += size.x;
			h = fmaxf(h, size.y);
		}
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
