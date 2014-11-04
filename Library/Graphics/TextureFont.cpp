#include "TextureFont.h"
#include "Image.h"
#include "TextureAtlas.h"
#include <codecvt>
#include <cstdlib>
#include <locale>


static bool ContainsArabic(const std::wstring& ws) { return false; }


TextureFont::TextureFont(TextureAtlas* textureAtlas, NSFont* font) :
	_textureAtlas(textureAtlas),
	_font(nil),
	_attributes(nil)

{
	_font = [font retain];
	_attributes = [@{
		NSFontAttributeName: _font,
		NSForegroundColorAttributeName: [NSColor whiteColor]
	} retain];

	/*
	#ifdef OPENWAR_USE_UIFONT
	_font = [[UIFont fontWithName:[NSString stringWithUTF8String:name] size:size] retain];
	#endif
	*/

/*
	_font1 = TTF_OpenFont(ANDROID_FONT1, (int)size);
	_font2 = TTF_OpenFont(ANDROID_FONT2, (int)size);
	_emoji = TTF_OpenFont(ANDROID_EMOJI, (int)size);

	if (_font1 != NULL)
	{
		TTF_SetFontStyle(_font1, TTF_STYLE_NORMAL);
		TTF_SetFontOutline(_font1, 0);
		TTF_SetFontKerning(_font1, 1);
		TTF_SetFontHinting(_font1, TTF_HINTING_LIGHT);
	}

	if (_font2 != NULL)
	{
		TTF_SetFontStyle(_font2, TTF_STYLE_NORMAL);
		TTF_SetFontOutline(_font2, 0);
		TTF_SetFontKerning(_font2, 1);
		TTF_SetFontHinting(_font2, TTF_HINTING_LIGHT);
	}

	if (_emoji != NULL)
	{
		TTF_SetFontStyle(_emoji, TTF_STYLE_NORMAL);
		TTF_SetFontOutline(_emoji, 0);
		TTF_SetFontKerning(_emoji, 1);
		TTF_SetFontHinting(_emoji, TTF_HINTING_LIGHT);
	}
	*/
}


TextureFont::~TextureFont()
{
	[_attributes release];
	[_font release];

/*#ifdef OPENWAR_USE_SDL
	if (_font1 != nullptr)
		TTF_CloseFont(_font1);

	if (_font2 != nullptr)
		TTF_CloseFont(_font2);

	if (_emoji != nullptr)
		TTF_CloseFont(_emoji);
#endif*/
}


TextureChar* TextureFont::GetTextureChar(const std::string& character)
{
	auto i = _textureChars.find(character);
	if (i != _textureChars.end())
		return i->second;

	NSString* string = [NSString stringWithUTF8String:character.c_str()];

	CGSize size = [string sizeWithAttributes:_attributes];

	int width = (int)size.width + 2;
	int height = (int)size.height + 2;


	Image* image = new Image(width, height);
	image->Fill(glm::vec4(0.2f, 0.2f, 0.2f, 1), bounds2f(0, 0, width, height));
	image->Fill(glm::vec4(0.3f, 0.3f, 0.3f, 1), bounds2f(1, 1, width - 1, height - 1));

	CGContextRef context = image->GetCGContext();

	NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:NO];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:gc];

	CGContextSetRGBFillColor(context, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor(context, 1, 1, 1, 1);

	CGPoint p = CGPointMake(1, 1);
	[string drawAtPoint:p withAttributes:_attributes];

	[NSGraphicsContext restoreGraphicsState];

	TextureImage* textureImage = _textureAtlas->AddTextureImage(image);
	textureImage->_inner.min = textureImage->_outer.min + glm::vec2(1, 1);
	textureImage->_inner.max = textureImage->_inner.min + glm::vec2(size.width, size.height);

	delete image;

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
		TextureChar* item = GetTextureChar(text);
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

			TextureChar* item = GetTextureChar(character);
			glm::vec2 size = item->GetInnerSize();
			w += size.x;
			h = fmaxf(h, size.y);
		}
	}

	return glm::vec2(w, h);
}



/*
#ifdef OPENWAR_USE_SDL

	SDL_Surface* image_surface = _image->GetSurface();

	SDL_FillRect(image_surface, NULL, SDL_MapRGBA(image_surface->format, 0, 0, 0, 0));

	SDL_Color color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;
	for (std::map<std::string, item>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		const item& item = (*i).second;

		SDL_Surface* surface = TTF_RenderUTF8_Blended(item._font, item._string.c_str(), color);
		if (surface != nullptr)
		{
			SDL_Rect rect;
			rect.x = (int)item._bounds_origin.x;
			rect.y = (int)item._bounds_origin.y;
			rect.w = (int)item._bounds_size.x;
			rect.h = (int)item._bounds_size.y;
			SDL_BlitSurface(surface, NULL, image_surface, &rect);
			SDL_FreeSurface(surface);
		}
	}

	_texture.LoadTextureFromImage(*_image);

#endif

#ifdef OPENWAR_USE_UIFONT
	UIGraphicsPushContext(_image->GetCGContext());

	CGContextClearRect(_image->GetCGContext(), CGRectMake(0, 0, _image->GetWidth(), _image->GetHeight()));

	for (std::map<std::string, item>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		const item& item = (*i).second;

		CGContextSetRGBFillColor(_image->GetCGContext(), 1, 1, 1, 1);
	    [item._string drawAtPoint:CGPointMake(item._bounds_origin.x, item._bounds_origin.y) withFont:item._font];
	}
	_texture.LoadTextureFromImage(*_image);

	UIGraphicsPopContext();

#endif

#ifdef OPENWAR_USE_NSFONT

	NSGraphicsContext *gc = [NSGraphicsContext graphicsContextWithGraphicsPort:_image->GetCGContext() flipped:YES];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:gc];

	CGContextClearRect(_image->GetCGContext(), CGRectMake(0, 0, _image->GetWidth(), _image->GetHeight()));

	for (std::map<std::string, item>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		const item& item = (*i).second;

		CGContextSetRGBFillColor(_image->GetCGContext(), 1, 1, 1, 1);
		NSDictionary* attributes = [NSDictionary dictionaryWithObjectsAndKeys:_font, NSFontAttributeName, nil];
		[item._string drawAtPoint:CGPointMake(item._bounds_origin.x, item._bounds_origin.y) withAttributes:attributes];

	}
	_textureAtlas.LoadTextureFromImage(*_image);

	[NSGraphicsContext restoreGraphicsState];

#endif
*/


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
