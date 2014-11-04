#include "TextureFont.h"
#include "Image.h"
#import "TextureAtlas.h"


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
}


TextureFont::~TextureFont()
{
	[_attributes release];
	[_font release];
}


TextureChar* TextureFont::GetTextureChar(const std::string& character)
{
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

	return new TextureChar(textureImage);
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
