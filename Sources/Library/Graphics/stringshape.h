#ifndef STRINGSHAPE_H
#define STRINGSHAPE_H

#include <map>

#ifndef OPENWAR_USE_SDL
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif
#endif

#include "../Algebra/bounds.h"
#include "shaderprogram.h"
#include "vertex.h"
#include "texture.h"


struct stringfont
{
	struct item
	{
		wchar_t _character;
		glm::vec2 _bounds_origin;
		glm::vec2 _bounds_size;
		float _u0, _u1;
		float _v0, _v1;
	};

	static image* _image;

#ifndef OPENWAR_USE_SDL
#if TARGET_OS_IPHONE
	UIFont* _font;
#else
	NSFont* _font;
#endif
#endif

	shaderprogram<texture_alpha_vertex>* _renderer;
	float _pixelDensity;
	texture _texture;

	std::map<wchar_t, item> _items;
	glm::vec2 _next;
	bool _dirty;

public:
	stringfont(const char* name, float size, float pixelDensity);
	stringfont(bool bold, float size, float pixelDensity);
	~stringfont();

private:
	void initialize();

public:
	float font_size() const;
	float shadow_offset() const;

	void add_character(wchar_t character);
	item get_character(wchar_t character) const;

	void update_texture();

	glm::vec2 measure(const char* string);
	glm::vec2 get_size(const item& item) const;
};



class stringshape
{
public:
	stringshape();
	~stringshape();
};


#endif
