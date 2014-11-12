#include "FontAdapter.h"
#include "GraphicsContext.h"
#include "Image.h"
#include "TextureAtlas.h"

#ifdef OPENWAR_USE_NSFONT
#import <AppKit/AppKit.h>
#endif


/*
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#define ANDROID_FONT1 "Roboto-Regular.ttf"
#define ANDROID_FONT2 "Roboto-Regular.ttf"
#define ANDROID_EMOJI "Roboto-Regular.ttf"
#else
#define ANDROID_FONT1 "/system/fonts/Roboto-Regular.ttf"
#define ANDROID_FONT2 "/system/fonts/DroidSansFallback.ttf"
#define ANDROID_EMOJI "/system/fonts/AndroidEmoji.ttf"
#endif
*/


/*
#ifdef OPENWAR_USE_UIFONT
	if (!fontDescriptor.name.empty())
		_font = [[UIFont fontWithName:[NSString stringWithUTF8String:fontDescriptor.name.c_str()] size:size] retain];
	else if (fontDescriptor.bold)
		_font = [[UIFont boldSystemFontOfSize:size] retain];
	else
		_font = [[UIFont systemFontOfSize:size] retain];

	_attributes = [@{
		//NSFontAttributeName: _font,
		//NSForegroundColorAttributeName: [UIColor whiteColor]
	} retain];
#endif
*/
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


/*#ifdef OPENWAR_USE_SDL
	if (_font1 != nullptr)
		TTF_CloseFont(_font1);

	if (_font2 != nullptr)
		TTF_CloseFont(_font2);

	if (_emoji != nullptr)
		TTF_CloseFont(_emoji);
#endif*/



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

FontAdapter::~FontAdapter()
{
}



/* FontAdapter_NSFont */


#ifdef OPENWAR_USE_NSFONT


FontAdapter_NSFont::FontAdapter_NSFont(GraphicsContext* gc, const FontDescriptor& fontDescriptor) :
	_font(nil),
	_attributes(nil)
{
	float size = fontDescriptor.size * gc->GetPixelDensity();
	if (size == 0)
		size = 12 * gc->GetPixelDensity();

	if (!fontDescriptor.name.empty())
		_font = [[NSFont fontWithName:[NSString stringWithUTF8String:fontDescriptor.name.c_str()] size:size] retain];
	else if (fontDescriptor.bold)
		_font = [[NSFont boldSystemFontOfSize:size] retain];
	else
		_font = [[NSFont systemFontOfSize:size] retain];

	_attributes = [@{
		NSFontAttributeName: _font,
		NSForegroundColorAttributeName: [NSColor whiteColor]
	} retain];

}


FontAdapter_NSFont::~FontAdapter_NSFont()
{
	[_attributes release];
	[_font release];
}


TextureImage* FontAdapter_NSFont::AddTextureImage(TextureAtlas* textureAtlas, const std::string& character, float blur)
{
	NSString* string = [NSString stringWithUTF8String:character.c_str()];

	CGSize size = [string sizeWithAttributes:_attributes];

	int border = (int)glm::ceil(blur) + 1;
	int width = (int)glm::ceil(size.width) + border * 2;
	int height = (int)glm::ceil(size.height) + border * 2;

	Image image(width, height);

	CGContextRef context = image.GetCGContext();

	NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:NO];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:gc];

	CGContextSetRGBFillColor(context, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor(context, 1, 1, 1, 1);

	CGPoint p = CGPointMake(border, border);
	[string drawAtPoint:p withAttributes:_attributes];

	[NSGraphicsContext restoreGraphicsState];

#ifdef OPENWAR_USE_UIFONT
	/*UIGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:NO];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:gc];*/

	CGContextSetRGBFillColor(context, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor(context, 1, 1, 1, 1);

	//CGPoint p = CGPointMake(1, 1);
	//[string drawAtPoint:p];

	/*[NSGraphicsContext restoreGraphicsState];*/
#endif

	if (blur != 0)
		image.Blur(blur);

	image.PremultiplyAlpha();

	TextureImage* textureImage = textureAtlas->AddTextureImage(image);
	textureImage->_inner.min = textureImage->_outer.min + glm::vec2(border, border);
	textureImage->_inner.max = textureImage->_inner.min + glm::vec2(size.width, size.height);

	return textureImage;
}


#endif
