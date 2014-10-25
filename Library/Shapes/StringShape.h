#ifndef STRINGSHAPE_H
#define STRINGSHAPE_H

#include <map>
#include <glm/gtc/matrix_transform.hpp>

#ifndef OPENWAR_USE_SDL
#if TARGET_OS_IPHONE
#define OPENWAR_USE_UIFONT
#else
#define OPENWAR_USE_NSFONT
#endif
#endif

#ifdef OPENWAR_USE_SDL
#include <SDL2_ttf/SDL_ttf.h>
#endif

#ifdef OPENWAR_USE_UIFONT
#import <UIKit/UIKit.h>
#endif

#ifdef OPENWAR_USE_NSFONT
#import <AppKit/AppKit.h>
#endif

#include "Algebra/bounds.h"
#include "ShaderProgram.h"
#include "texture.h"
#include "Shape.h"
#include "VertexShape.h"

class GraphicsContext;


class StringShader : public ShaderProgram3<glm::vec2, glm::vec2, float>
{
	friend class GraphicsContext;
	/*
		attribute vec2 position;
		attribute vec2 texcoord;
		attribute float alpha;
		uniform mat4 transform;
		uniform sampler2D texture;
		uniform vec4 color;
	 */
	StringShader(GraphicsContext* gc);
};


struct StringFont
{
#ifdef OPENWAR_USE_SDL
	typedef TTF_Font* font_ptr;
#endif
#ifdef OPENWAR_USE_UIFONT
	typedef UIFont* font_ptr;
#endif
#ifdef OPENWAR_USE_NSFONT
	typedef NSFont* font_ptr;
#endif

	struct item
	{
		font_ptr _font;
#ifdef OPENWAR_USE_SDL
		std::string _string;
#else
		NSString* _string;
#endif
		glm::vec2 _bounds_origin;
		glm::vec2 _bounds_size;
		float _u0, _u1;
		float _v0, _v1;
	};

	static Image* _image;

#ifdef OPENWAR_USE_SDL
	font_ptr _font1;
	font_ptr _font2;
	font_ptr _emoji;
#else
	font_ptr _font;
#endif

	ShaderProgram<Vertex_2f_2f_1f>* _renderer;
	float _pixelDensity;
	texture _texture;

	std::map<std::string, item> _items;
	glm::vec2 _next;
	bool _dirty;

public:
	StringFont(GraphicsContext* gc, const char* name, float size, float pixelDensity);
	StringFont(GraphicsContext* gc, bool bold, float size, float pixelDensity);
	~StringFont();

private:
	void initialize();

public:
	float font_size() const;
	float shadow_offset() const;

	font_ptr get_font_ptr() const;
	font_ptr get_font_ptr(wchar_t wc) const;

	item add_character(font_ptr font, const std::string& character);

	void update_texture();

	glm::vec2 measure(const char* text);
	glm::vec2 get_size(const item& item) const;

private:
	StringFont(const StringFont&) : _texture(nullptr) { }
	StringFont& operator=(const StringFont&) { return *this; }
};


class StringGlyph
{
	VertexGlyph<Vertex_2f_2f_1f> _glyph;
	std::string _string;
	glm::mat4x4 _transform;
	float _alpha;
	float _delta;

public:
	typedef Vertex_2f_2f_1f vertex_type;

	StringGlyph();
	StringGlyph(const char* string, glm::vec2 translate, float alpha = 1, float delta = 0);
	StringGlyph(const char* string, glm::mat4x4 transform, float alpha = 1, float delta = 0);

	const char* get_string() const { return _string.c_str(); }
	void set_string(const char* value) { _string = value; }

	const glm::mat4x4 get_transform() const { return _transform; }
	void set_transform(glm::mat4x4 value) { _transform = value; }
	void set_translate(glm::vec2 value) { _transform = glm::translate(glm::mat4(), glm::vec3(value, 0)); }

	const float get_alpha() const { return _alpha; }
	void set_alpha(float value) { _alpha = value; }

	const float get_delta() const { return _delta; }
	void set_delta(float value) { _delta = value; }

	VertexGlyph<Vertex_2f_2f_1f>* GetGlyph(StringFont* font);

	void generate(StringFont* font, std::vector<vertex_type>& vertices);

private:
	StringGlyph(const StringGlyph&) { }
	StringGlyph& operator=(const StringGlyph&) { return *this; }
};


class StringShape
{
	std::vector<StringGlyph*> _stringglyphs;

public:
	VertexShape<Vertex_2f_2f_1f> _vertices;
	StringFont* _font;

	explicit StringShape(StringFont* font);

	void clear();
	void add(const char* string, glm::mat4x4 transform, float alpha = 1, float delta = 0);

	void update(GLenum usage);

private:
	StringShape(const StringShape&) { }
	StringShape& operator=(const StringShape&) { return *this; }
};


#endif
