#include "stringshape.h"
#include "Algebra/image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <codecvt>
#include <cstdlib>
#include <locale>


#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#define ANDROID_FONT1 "Roboto-Regular.ttf"
#define ANDROID_FONT2 "Roboto-Regular.ttf"
#else
#define ANDROID_FONT1 "/system/fonts/Roboto-Regular.ttf"
#define ANDROID_FONT2 "/system/fonts/DroidSansFallback.ttf"
#define ANDROID_EMOJI "/system/fonts/AndroidEmoji.ttf"
#endif


static bool IsArabic(wchar_t wc)
{
	if (0x0600 <= wc && wc <= 0x08FF)
		return true;

	if (0xFB00 <= wc && wc <= 0xFDFF)
		return true;

	return false;
}


static bool ContainsArabic(const std::wstring& ws)
{
	for (wchar_t wc : ws)
		if (IsArabic(wc))
			return true;

	return false;
}


#if !defined(ENABLE_BIDIRECTIONAL_TEXT)
#define ENABLE_BIDIRECTIONAL_TEXT 0
//#error ENABLE_BIDIRECTIONAL_TEXT not defined
#endif

#if ENABLE_BIDIRECTIONAL_TEXT


#include "unicode/ubidi.h"


static unichar* ReserveSrcBuffer(int required)
{
	static unichar* buffer = nullptr;
	static int reserved = 0;

	if (buffer == nullptr || required > reserved)
	{
		delete buffer;
		buffer = new unichar[required];
		reserved = required;
	}

	return buffer;
}


static unichar* ReserveDstBuffer(int required)
{
	static unichar* buffer = nullptr;
	static int reserved = 0;

	if (buffer == nullptr || required > reserved)
	{
		delete buffer;
		buffer = new unichar[required];
		reserved = required;
	}

	return buffer;
}



static bool MayNeedReorder(unichar c)
{
	return c > 255;
}


static bool CanSkipReorder(NSString* string)
{
	NSUInteger length = string.length;
	if (length > 16)
		return false;

	static unichar buffer[16];
	[string getCharacters:buffer];

	for (NSUInteger i = 0; i < length; ++i)
		if (MayNeedReorder(buffer[i]))
			return false;

	return true;
}



static NSString* ReorderToDisplayDirection(NSString* string)
{
	if (CanSkipReorder(string))
		return string;

	UErrorCode error = U_ZERO_ERROR;
	int length = (int)string.length;

	unichar* src = ReserveSrcBuffer(length);
	unichar* dst = ReserveDstBuffer(length * 2);

	UBiDi* ubidi = ubidi_openSized(length, 0, &error);
    if (error != 0)
        NSLog(@"%04x", error);

	[string getCharacters:src];
    ubidi_setPara(ubidi, src, length, UBIDI_DEFAULT_LTR, NULL, &error);
    if (error != 0)
        NSLog(@"%04x", error);

	length = ubidi_writeReordered(ubidi, dst, length * 2, UBIDI_DO_MIRRORING | UBIDI_REMOVE_BIDI_CONTROLS, &error);
    if (error != 0)
        NSLog(@"%04x", error);

    NSString* result = [NSString stringWithCharacters:dst length:(NSUInteger)length];

    ubidi_close(ubidi);

	return result;
}

#endif




image* stringfont::_image = nullptr;



stringfont::stringfont(const char* name, float size, float pixelDensity) :
_font(0),
_pixelDensity(pixelDensity),
_items(),
_next(),
_dirty(false)
{
	initialize();

	size *= _pixelDensity;

#ifdef OPENWAR_USE_SDL
	_font = TTF_OpenFont(ANDROID_FONT1, size);
	if (_font  == NULL)
		_font = TTF_OpenFont(ANDROID_FONT2, size);

	if (_font != NULL)
	{
		TTF_SetFontStyle(_font, TTF_STYLE_NORMAL);
		TTF_SetFontOutline(_font, 0);
		TTF_SetFontKerning(_font, 1);
		TTF_SetFontHinting(_font, TTF_HINTING_LIGHT);
	}
#endif

#ifdef OPENWAR_USE_UIFONT
	_font = [[UIFont fontWithName:[NSString stringWithUTF8String:name] size:size] retain];
#endif

#ifdef OPENWAR_USE_NSFONT
	_font = [[NSFont fontWithName:[NSString stringWithUTF8String:name] size:size] retain];
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

#ifdef OPENWAR_USE_SDL
	_font = TTF_OpenFont(ANDROID_FONT1, size);
	if (_font  == NULL)
		_font = TTF_OpenFont(ANDROID_FONT2, size);

	if (_font != NULL)
	{
		TTF_SetFontStyle(_font, TTF_STYLE_NORMAL);
		TTF_SetFontOutline(_font, 0);
		TTF_SetFontKerning(_font, 1);
		TTF_SetFontHinting(_font, TTF_HINTING_LIGHT);
	}
#endif

#ifdef OPENWAR_USE_UIFONT
	if (bold)
		_font = [[UIFont boldSystemFontOfSize:size] retain];
	else
		_font = [[UIFont systemFontOfSize:size] retain];
#endif

#ifdef OPENWAR_USE_NSFONT
	if (bold)
		_font = [[NSFont boldSystemFontOfSize:size] retain];
	else
		_font = [[NSFont systemFontOfSize:size] retain];
#endif
}



stringfont::~stringfont()
{
#ifdef OPENWAR_USE_SDL
	if (_font != nullptr)
		TTF_CloseFont(_font);
#endif
#ifdef OPENWAR_USE_UIFONT
	[_font release];
#endif
#ifdef OPENWAR_USE_NSFONT
	[_font release];
#endif
}



void stringfont::initialize()
{
	if (_image == nullptr)
		_image = new image(1024, 512);

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




stringfont::item stringfont::add_character(const std::string& character)
{
	auto i = _items.find(character);
	if (i != _items.end())
		return i->second;


	glm::vec2 glyphsize;

#ifdef OPENWAR_USE_SDL
	int w, h;
	if (TTF_SizeUTF8(_font, character.c_str(), &w, &h) == 0)
	{
		glyphsize = glm::vec2(w, h);
	}
#endif

#ifdef OPENWAR_USE_UIFONT
	NSString* string = [NSString stringWithUTF8String:character.c_str()];
	CGSize size = [string sizeWithFont:_font];
	glyphsize = glm::vec2(size.width, size.height);
#endif

#ifdef OPENWAR_USE_NSFONT
	NSString* string = [NSString stringWithUTF8String:character.c_str()];
	NSDictionary* attributes = [NSDictionary dictionaryWithObjectsAndKeys:_font, NSFontAttributeName, nil];
	CGSize size = [string sizeWithAttributes:attributes];
	glyphsize = glm::vec2(size.width, size.height);
#endif

	if (_next.x + glyphsize.x > _image->width())
	{
		_next.x = 0;
		_next.y += glyphsize.y + 1;
		if (_next.y + glyphsize.y > _image->height())
		{
			_next.y = 0;

#ifndef OPENWAR_USE_SDL
			for (auto i : _items)
				[i.second._string release];
#endif

			_items.clear();
		}
	}

	item item;
#ifdef OPENWAR_USE_SDL
	item._string = character;
#else
	item._string = [string retain];
#endif
	item._bounds_origin = _next;
	item._bounds_size = glyphsize;
	item._u0 = item._bounds_origin.x / _image->width();
	item._u1 = (item._bounds_origin.x + item._bounds_size.x) / _image->width();
	item._v0 = 1 - (item._bounds_origin.y + item._bounds_size.y) / _image->height();
	item._v1 = 1 - item._bounds_origin.y / _image->height();

#ifdef OPENWAR_USE_SDL
	item._v0 = 1 - item._v0;
	item._v1 = 1 - item._v1;
#endif

	_items[character] = item;

	_next.x += floorf(item._bounds_size.x + 1) + 1;
	_dirty = true;

	return item;
}


void stringfont::update_texture()
{
	if (!_dirty)
		return;

#ifdef OPENWAR_USE_SDL

	SDL_Surface* image_surface = _image->get_surface();

	SDL_FillRect(image_surface, NULL, SDL_MapRGBA(image_surface->format, 0, 0, 0, 0));

	SDL_Color color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;
	for (std::map<std::string, item>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		const item& item = (*i).second;

		SDL_Surface* surface = TTF_RenderUTF8_Blended(_font, item._string.c_str(), color);
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

	_texture.load(*_image);

#endif

#ifdef OPENWAR_USE_UIFONT
	UIGraphicsPushContext(_image->CGContext());

	CGContextClearRect(_image->CGContext(), CGRectMake(0, 0, _image->width(), _image->height()));

	for (std::map<std::string, item>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		const item& item = (*i).second;

		CGContextSetRGBFillColor(_image->CGContext(), 1, 1, 1, 1);
	    [item._string drawAtPoint:CGPointMake(item._bounds_origin.x, item._bounds_origin.y) withFont:_font];
	}
	_texture.load(*_image);

	UIGraphicsPopContext();

#endif

#ifdef OPENWAR_USE_NSFONT
	NSGraphicsContext *gc = [NSGraphicsContext graphicsContextWithGraphicsPort:_image->CGContext() flipped:YES];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:gc];

	CGContextClearRect(_image->CGContext(), CGRectMake(0, 0, _image->width(), _image->height()));

	for (std::map<std::string, item>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		const item& item = (*i).second;

		CGContextSetRGBFillColor(_image->CGContext(), 1, 1, 1, 1);
		NSDictionary* attributes = [NSDictionary dictionaryWithObjectsAndKeys:_font, NSFontAttributeName, nil];
		[item._string drawAtPoint:CGPointMake(item._bounds_origin.x, item._bounds_origin.y) withAttributes:attributes];

	}
	_texture.load(*_image);

	[NSGraphicsContext restoreGraphicsState];

#endif

	_dirty = false;
}



glm::vec2 stringfont::measure(const char* text)
{
	float w = 0;
	float h = 0;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring ws = conv.from_bytes(text);

	if (ContainsArabic(ws))
	{
		stringfont::item item = add_character(text);
		glm::vec2 size = get_size(item);
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

			stringfont::item item = add_character(character);
			glm::vec2 size = get_size(item);
			w += size.x;
			h = fmaxf(h, size.y);
		}
	}

	return glm::vec2(w, h);
}



glm::vec2 stringfont::get_size(const item& item) const
{
	return glm::vec2(item._bounds_size.x, item._bounds_size.y) / _pixelDensity;

	/*float h = size;
	float w = item._bounds_size.x * size / item._bounds_size.y;
	return vector2(w, h) / pixel_scale();*/
}


/***/



stringglyph::stringglyph() :
_string(),
_transform(),
_alpha(1),
_delta(0)
{
}


stringglyph::stringglyph(const char* string, glm::vec2 translate, float alpha, float delta) :
_string(string),
_transform(glm::translate(glm::mat4(), glm::vec3(translate, 0))),
_alpha(alpha),
_delta(delta)
{
}


stringglyph::stringglyph(const char* string, glm::mat4x4 transform, float alpha, float delta) :
_string(string),
_transform(transform),
_alpha(alpha),
_delta(delta)
{
}


vertexglyph3<glm::vec2, glm::vec2, float> stringglyph::glyph(stringfont* font)
{
	return vertexglyph3<glm::vec2, glm::vec2, float>([this, font](std::vector<vertex3<glm::vec2, glm::vec2, float>>& vertices) {
		generate(font, vertices);
	});
}


void stringglyph::generate(stringfont* font, std::vector<stringglyph::vertex_type>& vertices)
{
#if ENABLE_BIDIRECTIONAL_TEXT
    string = ReorderToDisplayDirection(string);
#endif

	glm::vec2 p(0, 0);
	float alpha = _alpha;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring ws = conv.from_bytes(_string);

	if (ContainsArabic(ws))
	{
		stringfont::item item = font->add_character(_string.c_str());

		glm::vec2 s = font->get_size(item);
		bounds2f bounds = bounds2_from_corner(p, s);
		bounds.min = (_transform * glm::vec4(bounds.min.x, bounds.min.y, 0, 1)).xy();
		bounds.max = (_transform * glm::vec4(bounds.max.x, bounds.max.y, 0, 1)).xy();

		float next_alpha = alpha + _delta * s.x;

		vertices.push_back(stringglyph::vertex_type(bounds.p11(), glm::vec2(item._u0, item._v0), alpha));
		vertices.push_back(stringglyph::vertex_type(bounds.p12(), glm::vec2(item._u0, item._v1), alpha));
		vertices.push_back(stringglyph::vertex_type(bounds.p22(), glm::vec2(item._u1, item._v1), next_alpha));

		vertices.push_back(stringglyph::vertex_type(bounds.p22(), glm::vec2(item._u1, item._v1), next_alpha));
		vertices.push_back(stringglyph::vertex_type(bounds.p21(), glm::vec2(item._u1, item._v0), next_alpha));
		vertices.push_back(stringglyph::vertex_type(bounds.p11(), glm::vec2(item._u0, item._v0), alpha));
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

			stringfont::item item = font->add_character(character);

			glm::vec2 s = font->get_size(item);
			bounds2f bounds = bounds2_from_corner(p, s);
			bounds.min = (_transform * glm::vec4(bounds.min.x, bounds.min.y, 0, 1)).xy();
			bounds.max = (_transform * glm::vec4(bounds.max.x, bounds.max.y, 0, 1)).xy();

			float next_alpha = alpha + _delta * s.x;

			vertices.push_back(stringglyph::vertex_type(bounds.p11(), glm::vec2(item._u0, item._v0), alpha));
			vertices.push_back(stringglyph::vertex_type(bounds.p12(), glm::vec2(item._u0, item._v1), alpha));
			vertices.push_back(stringglyph::vertex_type(bounds.p22(), glm::vec2(item._u1, item._v1), next_alpha));

			vertices.push_back(stringglyph::vertex_type(bounds.p22(), glm::vec2(item._u1, item._v1), next_alpha));
			vertices.push_back(stringglyph::vertex_type(bounds.p21(), glm::vec2(item._u1, item._v0), next_alpha));
			vertices.push_back(stringglyph::vertex_type(bounds.p11(), glm::vec2(item._u0, item._v0), alpha));

			if (next_alpha < 0)
				break;

			p.x += s.x;
			alpha = next_alpha;
		}
	}
}


/***/



stringshape::stringshape(stringfont* font) : _font(font)
{
	_vbo._mode = GL_TRIANGLES;
}



void stringshape::clear()
{
	for (stringglyph* g : _stringglyphs)
		delete g;
	_stringglyphs.clear();

	glyphs.clear();
}




void stringshape::add(const char* s, glm::mat4x4 transform, float alpha, float delta)
{
	stringglyph* g = new stringglyph(s, transform, alpha, delta);
	_stringglyphs.push_back(g);

	glyphs.push_back(vertexglyph3<glm::vec2, glm::vec2, float>([this, g](std::vector<vertex3<glm::vec2, glm::vec2, float>>& vertices) {
		g->generate(_font, vertices);
	}));
}


void stringshape::update(GLenum usage)
{
	_font->update_texture();
	update_vbo();
}
