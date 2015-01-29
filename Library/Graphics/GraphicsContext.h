// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef GraphicsContext_H
#define GraphicsContext_H

#include <map>
#include <string>

#include "FontDescriptor.h"

#ifdef GRAPHICS_OPENGL_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif


#ifdef GRAPHICS_OPENGL_MAC
#include <OpenGL/gl.h>
//#define glGenVertexArraysOES glGenVertexArraysAPPLE
//#define glBindVertexArrayOES glBindVertexArrayAPPLE
//#define glDeleteVertexArraysOES glDeleteVertexArraysAPPLE
#endif


#ifdef GRAPHICS_OPENGL_ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif


#ifdef GRAPHICS_OPENGL_LINUX
#endif


#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif


#ifndef GL_POINT_SPRITE
#define GL_POINT_SPRITE 0x8861
#endif


class FontAdapter;
class ShaderProgram;
class TextureAtlas;

#define WIDGET_TEXTURE_ATLAS "WIDGET"


class GraphicsContext
{
	float _nativeScaling;
	float _virtualScaling;
	std::map<std::string, ShaderProgram*> _shaders;
	std::map<std::string, TextureAtlas*> _textureAtlases;
	std::map<FontDescriptor, FontAdapter*> _fontAdapters;

public:
	GraphicsContext(float nativeScaling, float virtualScaling);
	~GraphicsContext();

	GraphicsContext(const GraphicsContext&) = delete;
	GraphicsContext& operator=(const GraphicsContext&) = delete;

	float GetNativeScaling() const;
	float GetVirtualScaling() const;
	float GetCombinedScaling() const;

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

	TextureAtlas* GetTextureAtlas(const char* name);

	FontAdapter* GetFontAdapter(const FontDescriptor& fontDescriptor);
};


#endif
