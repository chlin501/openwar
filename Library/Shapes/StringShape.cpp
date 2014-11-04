#include "StringShape.h"
#include "Image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <codecvt>
#include <cstdlib>
#include <locale>

#include "TextureFont.h"


#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#define ANDROID_FONT1 "Roboto-Regular.ttf"
#define ANDROID_FONT2 "Roboto-Regular.ttf"
#define ANDROID_EMOJI "Roboto-Regular.ttf"
#else
#define ANDROID_FONT1 "/system/fonts/Roboto-Regular.ttf"
#define ANDROID_FONT2 "/system/fonts/DroidSansFallback.ttf"
#define ANDROID_EMOJI "/system/fonts/AndroidEmoji.ttf"
#endif



StringShader::StringShader(GraphicsContext* gc) : ShaderProgram3<glm::vec2, glm::vec2, float>(
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
			//result.rgb = color.rgb;
			//result.a = texture2D(texture, _texcoord).a * color.a * clamp(_alpha, 0.0, 1.0);
			result = texture2D(texture, _texcoord);

			gl_FragColor = result;
		}
	}))
{
	_blend_sfactor = GL_SRC_ALPHA;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


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




StringFont::StringFont(GraphicsContext* gc, const char* name, float size, float pixelDensity) :
	#ifdef OPENWAR_USE_SDL
	_font1(nullptr),
	_font2(nullptr),
	_emoji(nullptr),
	#else
	_font(0),
	#endif
	_pixelDensity(pixelDensity),
	_textureAtlas(gc),
	_textureFont(nullptr),
    _items(),
	_dirty(false)
{
	size *= _pixelDensity;

#ifdef OPENWAR_USE_SDL
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
#endif

#ifdef OPENWAR_USE_UIFONT
	_font = [[UIFont fontWithName:[NSString stringWithUTF8String:name] size:size] retain];
#endif

#ifdef OPENWAR_USE_NSFONT
	_font = [[NSFont fontWithName:[NSString stringWithUTF8String:name] size:size] retain];
#endif
}



StringFont::StringFont(GraphicsContext* gc, bool bold, float size, float pixelDensity) :
	#ifdef OPENWAR_USE_SDL
	_font1(nullptr),
	_font2(nullptr),
	_emoji(nullptr),
	#else
	_font(nil),
	#endif
	_pixelDensity(pixelDensity),
	_textureAtlas(gc),
	_textureFont(nullptr),
    _items(),
	_dirty(false)
{
	size *= _pixelDensity;

#ifdef OPENWAR_USE_SDL
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

	_textureFont = new TextureFont(&_textureAtlas, _font);
#endif
}



StringFont::~StringFont()
{
#ifdef OPENWAR_USE_SDL
	if (_font1 != nullptr)
		TTF_CloseFont(_font1);

	if (_font2 != nullptr)
		TTF_CloseFont(_font2);

	if (_emoji != nullptr)
		TTF_CloseFont(_emoji);
#endif

#ifdef OPENWAR_USE_UIFONT
	[_font release];
#endif

#ifdef OPENWAR_USE_NSFONT
	[_font release];
#endif
}



StringFont::font_ptr StringFont::get_font_ptr() const
{
#ifdef OPENWAR_USE_SDL
	return _font2;
#else
	return _font;
#endif
}


StringFont::font_ptr StringFont::get_font_ptr(wchar_t wc) const
{
#ifdef OPENWAR_USE_SDL
	if (_emoji != nullptr && TTF_GlyphIsProvided(_emoji, (Uint16)wc))
		return _emoji;
	if (_font1 != nullptr && TTF_GlyphIsProvided(_font1, (Uint16)wc))
		return _font1;
	return _font2;
#else
	return _font;
#endif
}


TextureChar* StringFont::add_character(font_ptr font, const std::string& character)
{
	auto i = _items.find(character);
	if (i != _items.end())
		return i->second;

	TextureChar* textureChar = _textureFont->GetTextureChar(character);

	_items[character] = textureChar;
	_dirty = true;

	return textureChar;
}


void StringFont::update_texture()
{
	if (_dirty)
	{
		_textureAtlas.UpdateTextureAtlas();
		_dirty = false;
	}
}



glm::vec2 StringFont::measure(const char* text)
{
	float w = 0;
	float h = 0;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv(".", L".");
	std::wstring ws = conv.from_bytes(text);

	if (ContainsArabic(ws))
	{
		TextureChar* item = add_character(get_font_ptr(), text);
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

			TextureChar* item = add_character(get_font_ptr(wc), character);
			glm::vec2 size = item->GetInnerSize();
			w += size.x;
			h = fmaxf(h, size.y);
		}
	}

	return glm::vec2(w, h);
}


/***/



StringGlyph::StringGlyph() :
	_shape(nullptr),
	_string(),
	_transform(),
	_alpha(1),
	_delta(0)
{
}


StringGlyph::StringGlyph(const char* string, glm::vec2 translate, float alpha, float delta) :
	_shape(nullptr),
	_string(string),
	_transform(glm::translate(glm::mat4(), glm::vec3(translate, 0))),
	_alpha(alpha),
	_delta(delta)
{
}


StringGlyph::StringGlyph(const char* string, glm::mat4x4 transform, float alpha, float delta) :
	_shape(nullptr),
	_string(string),
	_transform(transform),
	_alpha(alpha),
	_delta(delta)
{
}


StringGlyph::~StringGlyph()
{
	if (_shape != nullptr)
		_shape->RemoveGlyph(this);
}



/***/



StringShape::StringVertexBuffer::StringVertexBuffer(StringShape* shape) :
	_shape(shape)
{
}


void StringShape::StringVertexBuffer::Update()
{
	_shape->UpdateVertexBuffer();
}


/***/



StringShape::StringShape(StringFont* font) :
	_vertices(this),
	_font(font)
{
	_vertices._mode = GL_TRIANGLES;
}


StringShape::~StringShape()
{
	for (StringGlyph* glyph : _glyphs)
		glyph->_shape = nullptr;
}


VertexBuffer<Vertex_2f_2f_1f>* StringShape::GetVertices()
{
	return &_vertices;
}


void StringShape::ClearGlyphs()
{
	for (StringGlyph* glyph : _glyphs)
		glyph->_shape = nullptr;
	_glyphs.clear();
}


void StringShape::AddGlyph(StringGlyph* glyph)
{
	if (glyph->_shape != nullptr)
		glyph->_shape->RemoveGlyph(glyph);

	glyph->_shape = this;
	_glyphs.push_back(glyph);
}


void StringShape::RemoveGlyph(StringGlyph* glyph)
{
	glyph->_shape = nullptr;
	_glyphs.erase(
		std::remove(_glyphs.begin(), _glyphs.end(), glyph),
		_glyphs.end());
}


void StringShape::UpdateVertexBuffer()
{
	static std::vector<Vertex_2f_2f_1f> vertices;

	for (StringGlyph* glyph : _glyphs)
		AppendStringGlyph(vertices, glyph);

	_vertices.UpdateVBO(GL_TRIANGLES, vertices.data(), vertices.size());
	vertices.clear();

	_font->update_texture();
}


void StringShape::AppendStringGlyph(std::vector<Vertex_2f_2f_1f>& vertices, StringGlyph* glyph)
{
	glm::vec2 p(0, 0);
	float alpha = glyph->_alpha;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv(".", L".");
	std::wstring ws = conv.from_bytes(glyph->_string);

	if (ContainsArabic(ws))
	{
		TextureChar* textureChar = _font->add_character(_font->get_font_ptr(), glyph->_string.c_str());

		bounds2f item_xy = textureChar->GetOuterXY(p);
		bounds2f item_uv = textureChar->GetOuterUV();
		float item_u0 = item_uv.min.x;
		float item_u1 = item_uv.max.x;
		float item_v0 = item_uv.min.y;
		float item_v1 = item_uv.max.y;

		glm::vec2 s = textureChar->GetInnerSize();

		bounds2f bounds;
		bounds.min = (glyph->_transform * glm::vec4(item_xy.min.x, item_xy.min.y, 0, 1)).xy();
		bounds.max = (glyph->_transform * glm::vec4(item_xy.max.x, item_xy.max.y, 0, 1)).xy();

		float next_alpha = alpha + glyph->_delta * s.x;

		vertices.push_back(Vertex_2f_2f_1f(bounds.p11(), glm::vec2(item_u0, item_v1), alpha));
		vertices.push_back(Vertex_2f_2f_1f(bounds.p12(), glm::vec2(item_u0, item_v0), alpha));
		vertices.push_back(Vertex_2f_2f_1f(bounds.p22(), glm::vec2(item_u1, item_v0), next_alpha));
		vertices.push_back(Vertex_2f_2f_1f(bounds.p22(), glm::vec2(item_u1, item_v0), next_alpha));
		vertices.push_back(Vertex_2f_2f_1f(bounds.p21(), glm::vec2(item_u1, item_v1), next_alpha));
		vertices.push_back(Vertex_2f_2f_1f(bounds.p11(), glm::vec2(item_u0, item_v1), alpha));
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

			TextureChar* textureChar = _font->add_character(_font->get_font_ptr(wc), character);


			bounds2f item_xy = textureChar->GetOuterXY(p);
			bounds2f item_uv = textureChar->GetOuterUV();
			float item_u0 = item_uv.min.x;
			float item_u1 = item_uv.max.x;
			float item_v0 = item_uv.min.y;
			float item_v1 = item_uv.max.y;

			glm::vec2 s = textureChar->GetInnerSize();

			bounds2f bounds;
			bounds.min = (glyph->_transform * glm::vec4(item_xy.min.x, item_xy.min.y, 0, 1)).xy();
			bounds.max = (glyph->_transform * glm::vec4(item_xy.max.x, item_xy.max.y, 0, 1)).xy();

			float next_alpha = alpha + glyph->_delta * s.x;

			vertices.push_back(Vertex_2f_2f_1f(bounds.p11(), glm::vec2(item_u0, item_v1), alpha));
			vertices.push_back(Vertex_2f_2f_1f(bounds.p12(), glm::vec2(item_u0, item_v0), alpha));
			vertices.push_back(Vertex_2f_2f_1f(bounds.p22(), glm::vec2(item_u1, item_v0), next_alpha));
			vertices.push_back(Vertex_2f_2f_1f(bounds.p22(), glm::vec2(item_u1, item_v0), next_alpha));
			vertices.push_back(Vertex_2f_2f_1f(bounds.p21(), glm::vec2(item_u1, item_v1), next_alpha));
			vertices.push_back(Vertex_2f_2f_1f(bounds.p11(), glm::vec2(item_u0, item_v1), alpha));

			if (next_alpha < 0)
				break;

			p.x += s.x;
			p.x = std::ceilf(p.x);

			alpha = next_alpha;
		}
	}
}
