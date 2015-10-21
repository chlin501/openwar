// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef GraphicsContext_H
#define GraphicsContext_H

#include <map>
#include <string>

#include "FontAdapter.h"
#include "FontDescriptor.h"
#include "Algebra/bounds.h"


#ifdef OPENWAR_PLATFORM_IOS
#define OPENWAR_USE_GLES2
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif


#ifdef OPENWAR_PLATFORM_MAC
#include <OpenGL/gl3.h>
//#define glGenVertexArraysOES glGenVertexArraysAPPLE
//#define glBindVertexArrayOES glBindVertexArrayAPPLE
//#define glDeleteVertexArraysOES glDeleteVertexArraysAPPLE
#endif


#ifdef OPENWAR_PLATFORM_ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif


#ifdef OPENWAR_PLATFORM_LINUX
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif


#ifdef OPENWAR_PLATFORM_WEB
#define OPENWAR_USE_GLES2
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif


#if defined(OPENWAR_PLATFORM_IOS) || defined(OPENWAR_PLATFORM_MAC)
#import <CoreGraphics/CoreGraphics.h>
#endif



#ifdef OPENWAR_USE_SDL
#ifdef OPENWAR_PLATFORM_WEB
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#else
#include <SDL2/SDL.h>
#ifdef OPENWAR_PLATFORM_LINUX
#include <SDL2/SDL_image.h>
#else
#include <SDL2_image/SDL_image.h>
#endif
#endif
#endif

#ifdef OPENWAR_USE_FONTADAPTER_SDLTTF
#include <SDL2/SDL_ttf.h>
#endif



#define CHECK_OPENGL_ERROR() CheckOpenGLError(__FILE__, __LINE__)

extern void CheckOpenGLError(const char* file, int line);


#ifndef GL_VERTEX_PROGRAM_POINT_SIZE
#define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
#endif

#ifndef GL_POINT_SPRITE
#define GL_POINT_SPRITE 0x8861
#endif


class FontAdapter;
class FrameBuffer;
class ShaderProgram;
class TextureAtlas;

#define WIDGET_TEXTURE_ATLAS "WIDGET"


class GraphicsContext
{
	float _nativeScaling{};
	float _virtualScaling{};
	std::map<std::string, ShaderProgram*> _shaders{};
	std::map<std::string, TextureAtlas*> _textureAtlases{};
	std::map<FontDescriptor, FontAdapter*> _fontAdapters{};

public:
	GraphicsContext(float nativeScaling, float virtualScaling);
	~GraphicsContext();

	GraphicsContext(const GraphicsContext&) = delete;
	GraphicsContext& operator=(const GraphicsContext&) = delete;

	float GetNativeScaling() const;
	float GetVirtualScaling() const;
	float GetCombinedScaling() const;

	bounds2i GetViewportBounds() const;

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
