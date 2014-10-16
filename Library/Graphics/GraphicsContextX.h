// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef GraphicsContext_H
#define GraphicsContext_H

#include <map>
#include "ShaderProgramX.h"


class GraphicsContext
{
	static int _shaderprogram_id;

	float _pixeldensity;

	std::map<int, ShaderProgramBase*> _shaderprograms;

public:
	GraphicsContext(float pixelDensity);
	~GraphicsContext();

	float GetPixelDensity() const { return _pixeldensity; }

	static int generate_shaderprogram_id();

	template <class T1>
	ShaderProgram1<T1>* LoadShaderProgram1(int id, const char* a1, const char* vertexshader, const char* fragmentshader)
	{
		auto i = _shaderprograms.find(id);
		if (i != _shaderprograms.end())
			return dynamic_cast<ShaderProgram1<T1>*>(i->second);

		ShaderProgram1<T1>* result = new ShaderProgram1<T1>(a1, vertexshader, fragmentshader);
		_shaderprograms[id] = result;
		return result;
	}

	template <class T1, class T2>
	ShaderProgram2<T1, T2>* LoadShaderProgram2(int id, const char* a1, const char* a2, const char* vertexshader, const char* fragmentshader)
	{
		auto i = _shaderprograms.find(id);
		if (i != _shaderprograms.end())
			return dynamic_cast<ShaderProgram2<T1, T2>*>(i->second);

		ShaderProgram2<T1, T2>* result = new ShaderProgram2<T1, T2>(a1, a2, vertexshader, fragmentshader);
		_shaderprograms[id] = result;
		return result;
	}

	template <class T1, class T2, class T3>
	ShaderProgram3<T1, T2, T3>* LoadShaderProgram3(int id, const char* a1, const char* a2, const char* a3, const char* vertexshader, const char* fragmentshader)
	{
		auto i = _shaderprograms.find(id);
		if (i != _shaderprograms.end())
			return dynamic_cast<ShaderProgram3<T1, T2, T3>*>(i->second);

		ShaderProgram3<T1, T2, T3>* result = new ShaderProgram3<T1, T2, T3>(a1, a2, a3, vertexshader, fragmentshader);
		_shaderprograms[id] = result;
		return result;
	}

	template <class T1, class T2, class T3, class T4>
	ShaderProgram4<T1, T2, T3, T4>* LoadShaderProgram4(int id, const char* a1, const char* a2, const char* a3, const char* a4, const char* vertexshader, const char* fragmentshader)
	{
		auto i = _shaderprograms.find(id);
		if (i != _shaderprograms.end())
			return dynamic_cast<ShaderProgram4<T1, T2, T3, T4>*>(i->second);

		ShaderProgram4<T1, T2, T3, T4>* result = new ShaderProgram4<T1, T2, T3, T4>(a1, a2, a3, a4, vertexshader, fragmentshader);
		_shaderprograms[id] = result;
		return result;
	}
};


#endif
