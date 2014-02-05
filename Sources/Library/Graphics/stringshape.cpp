#include "stringshape.h"
#include "../Algebra/image.h"




image* stringfont::_image = nullptr;



stringfont::stringfont(const char* name, float size, float pixelDensity) :
#ifndef OPENWAR_USE_SDL
_font(nil),
#endif
_pixelDensity(pixelDensity),
_items(),
_next(),
_dirty(false)
{
	initialize();

	size *= _pixelDensity;

#ifndef OPENWAR_USE_SDL
#if TARGET_OS_IPHONE
	_font = [[UIFont fontWithName:[NSString stringWithUTF8String:name] size:size] retain];
#else
	_font = [[NSFont fontWithName:[NSString stringWithUTF8String:name] size:size] retain];
#endif
#endif
}



stringfont::stringfont(bool bold, float size, float pixelDensity) :
#ifndef OPENWAR_USE_SDL
_font(nil),
#endif
_pixelDensity(pixelDensity),
_items(),
_next(),
_dirty(false)
{
	initialize();

	size *= _pixelDensity;

#ifndef OPENWAR_USE_SDL
#if TARGET_OS_IPHONE
	if (bold)
		_font = [[UIFont boldSystemFontOfSize:size] retain];
	else
		_font = [[UIFont systemFontOfSize:size] retain];
#else
	if (bold)
		_font = [[NSFont boldSystemFontOfSize:size] retain];
	else
		_font = [[NSFont systemFontOfSize:size] retain];
#endif
#endif
}



stringfont::~stringfont()
{
#ifndef OPENWAR_USE_SDL
	[_font release];
#endif
}



void stringfont::initialize()
{
	if (_image == nullptr)
		_image = new image(512, 512);

	_renderer = new shaderprogram3<glm::vec2, glm::vec2, float>(
		"position", "texcoord", "alpha",
		VERTEX_SHADER
		({
			attribute vec2 position;
			attribute vec2 texcoord;
			attribute float alpha;
			uniform mat4 transform;
			varying vec2 _texcoord;
			varying float _alpha;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				_texcoord = texcoord;
				_alpha = alpha;

				gl_Position = vec4(p.x, p.y, 0.5, 1.0);
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			uniform vec4 color;
			varying vec2 _texcoord;
			varying float _alpha;

			void main()
			{
				vec4 result;
				result.rgb = color.rgb;
				result.a = texture2D(texture, _texcoord).a * color.a * clamp(_alpha, 0.0, 1.0);

				gl_FragColor = result;
			}
		})
	);
	_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}



float stringfont::font_size() const
{
#ifdef OPENWAR_USE_SDL

	return 14;

#else

	return (float)_font.pointSize / _pixelDensity;

#endif
}



float stringfont::shadow_offset() const
{
	return 1 / _pixelDensity;
}




void stringfont::add_character(wchar_t character)
{
	if (_items.find(character) != _items.end())
		return;

#ifndef OPENWAR_USE_SDL

	unichar uc = (unichar)character;

	NSString* text = [NSString stringWithCharacters:&uc length:1];

#if TARGET_OS_IPHONE
	CGSize size = [text sizeWithFont:_font];
#else
	NSDictionary* attributes = [NSDictionary dictionaryWithObjectsAndKeys:_font, NSFontAttributeName, nil];
	CGSize size = [text sizeWithAttributes:attributes];
#endif

	if (_next.x + size.width > _image->width())
	{
		_next.x = 0;
		_next.y += size.height + 1;
		if (_next.y + size.height > _image->height())
			_next.y = 0; // TODO: handle texture resizing
	}

	item item;
	item._character = character;
	item._bounds_origin = _next;
	item._bounds_size = glm::vec2(size.width, size.height);
	item._u0 = (float)item._bounds_origin.x / _image->width();
	item._u1 = (float)(item._bounds_origin.x + item._bounds_size.x) / _image->width();
	item._v0 = 1 - (float)(item._bounds_origin.y + item._bounds_size.y) / _image->height();
	item._v1 = 1 - (float)item._bounds_origin.y / _image->height();

	_items[character] = item;

	_next.x += floorf((float)item._bounds_size.x + 1) + 1;
	_dirty = true;

#endif
}



stringfont::item stringfont::get_character(wchar_t character) const
{
	auto i = _items.find(character);
	if (i != _items.end())
		return i->second;

	return _items.find('.')->second;
}



void stringfont::update_texture()
{
	if (!_dirty)
		return;

#ifdef OPENWAR_USE_SDL

#else

#if TARGET_OS_IPHONE
	UIGraphicsPushContext(_image->CGContext());
#else
	NSGraphicsContext *gc = [NSGraphicsContext graphicsContextWithGraphicsPort:_image->CGContext() flipped:YES];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:gc];
#endif

	CGContextClearRect(_image->CGContext(), CGRectMake(0, 0, _image->width(), _image->height()));

	for (std::map<wchar_t, item>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		item item = (*i).second;

		unichar uc = (unichar)item._character;
		NSString *text = [NSString stringWithCharacters:&uc length:1];

		CGContextSetRGBFillColor(_image->CGContext(), 1, 1, 1, 1);

#if TARGET_OS_IPHONE
	    [text drawAtPoint:CGPointMake(item._bounds_origin.x, item._bounds_origin.y) withFont:_font];
#else
		NSDictionary* attributes = [NSDictionary dictionaryWithObjectsAndKeys:_font, NSFontAttributeName, nil];
		[text drawAtPoint:CGPointMake(item._bounds_origin.x, item._bounds_origin.y) withAttributes:attributes];
#endif
	}

	_texture.load(*_image);

#if TARGET_OS_IPHONE
	UIGraphicsPopContext();
#else
	[NSGraphicsContext restoreGraphicsState];
#endif

#endif

	_dirty = false;
}



glm::vec2 stringfont::measure(const char* s)
{
	float w = 0;
	float h = 0;

#ifndef OPENWAR_USE_SDL

	NSString* string = [NSString stringWithUTF8String:s];

	//return vector2([string sizeWithFont:_font]) * size / _font.pointSize;

	for (NSUInteger i = 0; i < string.length; ++i)
	{
		wchar_t character = [string characterAtIndex:i];
		add_character(character);
	}

	for (NSUInteger i = 0; i < string.length; ++i)
	{
		wchar_t character = [string characterAtIndex:i];
		item item = get_character(character);
		glm::vec2 s = get_size(item);
		w += s.x;
		h = fmaxf(h, s.y);
	}

#endif

	return glm::vec2(w, h);
}



glm::vec2 stringfont::get_size(const item& item) const
{
	return glm::vec2(item._bounds_size.x, item._bounds_size.y) / _pixelDensity;

	/*float h = size;
	float w = item._bounds_size.x * size / item._bounds_size.y;
	return vector2(w, h) / pixel_scale();*/
}





stringshape::stringshape()
{
}


stringshape::~stringshape()
{
}
