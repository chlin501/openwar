// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef STRING_SHAPE_H
#define STRING_SHAPE_H

#include "stringshape.h"


class string_shape
{
public:
	vertexbuffer<texture_alpha_vertex> _vbo;
	stringfont* _font;

	string_shape(stringfont* font);

	void clear();
	void add(const char* string, glm::mat4x4 transform, float alpha = 1, float delta = 0);

	void update(GLenum usage);
};



#endif
