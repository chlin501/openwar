// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef GraphicsContext_H
#define GraphicsContext_H

#include "FontAdapter.h"
#include "GraphicsOpenGL.h"
#include "ShaderProgram.h"
#include <map>
#include <string>

class TextureAtlas;


class GraphicsContext
{
	float _nativeScaling;
	float _virtualScaling;
	std::map<std::string, ShaderProgram*> _shaders;
	mutable TextureAtlas* _widgetTextureAtlas;
	std::map<FontDescriptor, FontAdapter*> _fontAdapters;

public:
	GraphicsContext(float nativeScaling, float virtualScaling);
	~GraphicsContext();

	float GetNativeScaling() const;
	float GetVirtualScaling() const;
	float GetCombinedScaling() const;

	TextureAtlas* GetWidgetTextureAtlas() const;

	FontAdapter* GetFontAdapter(const FontDescriptor& fontDescriptor);

	template <class _ShaderProgram> _ShaderProgram* GetShaderProgram()
	{
		std::string name = typeid(_ShaderProgram).name();
		_ShaderProgram* result = dynamic_cast<_ShaderProgram*>(_shaders[name]);
		if (result == nullptr)
		{
			result = new _ShaderProgram(this);
			_shaders[name] = result;
		}
		return result;
	}

private:
	GraphicsContext(const GraphicsContext&) { }
	GraphicsContext& operator=(const GraphicsContext&) { return *this; }
};


#endif
