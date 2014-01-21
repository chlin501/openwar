// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

#include <map>
#include "shaderprogram.h"


class shaderprogram_id
{
};


class graphicscontext
{
	float _pixeldensity;

	std::map<shaderprogram_id, shaderprogram_base> _shaderprograms;

public:
	graphicscontext(float pixelDensity);
	~graphicscontext();

	float get_pixeldensity() const { return _pixeldensity; }

	template <class T1>
	shaderprogram1<T1>* load_shaderprogram(shaderprogram_id id, const char* a1, const char* vertexshader, const char* fragmentshader)
	{
		return nullptr;
	}
};


#endif
