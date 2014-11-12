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


TextureChar* TextureFont::GetTextureChar(const std::string& character, float blur)
{
	auto i = _textureChars.find(character);
	if (i != _textureChars.end())
		return i->second;

	TextureImage* textureImage = _fontAdapter->AddTextureImage(_textureAtlas, character, blur);
	TextureChar* textureChar = new TextureChar(textureImage);
	_textureChars[character] = textureChar;
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


TextureChar::TextureChar(TextureImage* textureImage) :
	_textureImage(textureImage)
{

}


glm::vec2 TextureChar::GetInnerSize() const
{
	return _textureImage->GetInnerBounds().size();
}


bounds2f TextureChar::GetOuterUV() const
{
	return _textureImage->GetOuterUV();
}


bounds2f TextureChar::GetOuterXY(glm::vec2 position) const
{
	bounds2f inner_xy = _textureImage->GetInnerBounds();
	bounds2f outer_xy = _textureImage->GetOuterBounds();
	return outer_xy + position - inner_xy.min;
}


TextureImage* TextureChar::GetTextureImage() const
{
	return _textureImage;
}
