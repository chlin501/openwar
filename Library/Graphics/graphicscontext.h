// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

#include <map>
#include "shaderprogram.h"


class graphicscontext
{
	static int _shaderprogram_id;

	float _pixeldensity;

	std::map<int, shaderprogram_base*> _shaderprograms;

public:
	graphicscontext(float pixelDensity);
	~graphicscontext();

	float get_pixeldensity() const { return _pixeldensity; }

	static int generate_shaderprogram_id();

	template <class T1>
	shaderprogram1<T1>* load_shaderprogram1(int id, const char* a1, const char* vertexshader, const char* fragmentshader)
	{
		auto i = _shaderprograms.find(id);
		if (i != _shaderprograms.end())
			return dynamic_cast<shaderprogram1<T1>*>(i->second);

		shaderprogram1<T1>* result = new shaderprogram1<T1>(a1, vertexshader, fragmentshader);
		_shaderprograms[id] = result;
		return result;
	}

	template <class T1, class T2>
	shaderprogram2<T1, T2>* load_shaderprogram2(int id, const char* a1, const char* a2, const char* vertexshader, const char* fragmentshader)
	{
		auto i = _shaderprograms.find(id);
		if (i != _shaderprograms.end())
			return dynamic_cast<shaderprogram2<T1, T2>*>(i->second);

		shaderprogram2<T1, T2>* result = new shaderprogram2<T1, T2>(a1, a2, vertexshader, fragmentshader);
		_shaderprograms[id] = result;
		return result;
	}

	template <class T1, class T2, class T3>
	shaderprogram3<T1, T2, T3>* load_shaderprogram3(int id, const char* a1, const char* a2, const char* a3, const char* vertexshader, const char* fragmentshader)
	{
		auto i = _shaderprograms.find(id);
		if (i != _shaderprograms.end())
			return dynamic_cast<shaderprogram3<T1, T2, T3>*>(i->second);

		shaderprogram3<T1, T2, T3>* result = new shaderprogram3<T1, T2, T3>(a1, a2, a3, vertexshader, fragmentshader);
		_shaderprograms[id] = result;
		return result;
	}

	template <class T1, class T2, class T3, class T4>
	shaderprogram4<T1, T2, T3, T4>* load_shaderprogram4(int id, const char* a1, const char* a2, const char* a3, const char* a4, const char* vertexshader, const char* fragmentshader)
	{
		auto i = _shaderprograms.find(id);
		if (i != _shaderprograms.end())
			return dynamic_cast<shaderprogram4<T1, T2, T3, T4>*>(i->second);

		shaderprogram4<T1, T2, T3, T4>* result = new shaderprogram4<T1, T2, T3, T4>(a1, a2, a3, a4, vertexshader, fragmentshader);
		_shaderprograms[id] = result;
		return result;
	}
};


#endif
