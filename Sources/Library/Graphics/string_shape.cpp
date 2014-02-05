// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "string_shape.h"


/***/



string_shape::string_shape(stringfont* font) : _font(font)
{
	_vbo._mode = GL_TRIANGLES;
}



void string_shape::clear()
{
	_vbo._vertices.clear();
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



void string_shape::add(const char* s, glm::mat4x4 transform, float alpha, float delta)
{
#ifndef OPENWAR_USE_SDL

	NSString* string = [NSString stringWithUTF8String:s];

#if ENABLE_BIDIRECTIONAL_TEXT
    string = ReorderToDisplayDirection(string);
    #endif

	for (NSUInteger i = 0; i < string.length; ++i)
	{
		wchar_t character = [string characterAtIndex:i];
		_font->add_character(character);
	}

	glm::vec2 p(0, 0);

	for (NSUInteger i = 0; i < string.length; ++i)
	{
		wchar_t character = [string characterAtIndex:i];
		stringfont::item item = _font->get_character(character);

		glm::vec2 s = _font->get_size(item);
		bounds2f bounds = bounds2_from_corner(p, s);
		bounds.min = (transform * glm::vec4(bounds.min.x, bounds.min.y, 0, 1)).xy();
		bounds.max = (transform * glm::vec4(bounds.max.x, bounds.max.y, 0, 1)).xy();

		float next_alpha = alpha + delta * s.x;

		_vbo._vertices.push_back(texture_alpha_vertex(bounds.p11(), glm::vec2(item._u0, item._v0), alpha));
		_vbo._vertices.push_back(texture_alpha_vertex(bounds.p12(), glm::vec2(item._u0, item._v1), alpha));
		_vbo._vertices.push_back(texture_alpha_vertex(bounds.p22(), glm::vec2(item._u1, item._v1), next_alpha));

		_vbo._vertices.push_back(texture_alpha_vertex(bounds.p22(), glm::vec2(item._u1, item._v1), next_alpha));
		_vbo._vertices.push_back(texture_alpha_vertex(bounds.p21(), glm::vec2(item._u1, item._v0), next_alpha));
		_vbo._vertices.push_back(texture_alpha_vertex(bounds.p11(), glm::vec2(item._u0, item._v0), alpha));

		if (next_alpha < 0)
			break;

		p.x += s.x;
		alpha = next_alpha;
	}

#endif
}


void string_shape::update(GLenum usage)
{
	_font->update_texture();
	_vbo.update(usage);
}
