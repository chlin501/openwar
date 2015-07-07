// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "GraphicsContext.h"
#include "TextureAtlas.h"
#include "ShaderProgram.h"
#include "FontAdapter.h"
#include "FrameBuffer.h"


GraphicsContext::GraphicsContext(float nativeScaling, float virtualScaling) :
	_nativeScaling(nativeScaling),
	_virtualScaling(virtualScaling)
{
#if !defined(PHALANX_USING_GLES2)
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);
#endif
}


GraphicsContext::~GraphicsContext()
{
	for (auto i : _fontAdapters)
		delete i.second;

	for (auto i : _textureAtlases)
		delete i.second;
}


float GraphicsContext::GetNativeScaling() const
{
	return _nativeScaling;
}


float GraphicsContext::GetVirtualScaling() const
{
	return _virtualScaling;
}


float GraphicsContext::GetCombinedScaling() const
{
	return _nativeScaling * _virtualScaling;
}


bounds2i GraphicsContext::GetViewportBounds() const
{
	GLint v[4];
	glGetIntegerv(GL_VIEWPORT, v);
	return bounds2i{v[0], v[1], v[0] + v[2], v[1] + v[3]};
}


TextureAtlas* GraphicsContext::GetTextureAtlas(const char* name)
{
	auto i = _textureAtlases.find(name);
	if (i != _textureAtlases.end())
		return i->second;

	TextureAtlas* result = new TextureAtlas(this);
	_textureAtlases[name] = result;
	return result;
}


FontAdapter* GraphicsContext::GetFontAdapter(const FontDescriptor& fontDescriptor)
{
	auto i = _fontAdapters.find(fontDescriptor);
	if (i != _fontAdapters.end())
		return i->second;

	FontAdapter* fontAdapter = nullptr;

#ifdef PHALANX_USING_FONTADAPTER_NSFONT
	if (!fontAdapter)
		fontAdapter = new FontAdapter_NSFont(this, fontDescriptor);
#endif

#ifdef PHALANX_USING_FONTADAPTER_UIFONT
	if (!fontAdapter)
		fontAdapter = new FontAdapter_UIFont(this, fontDescriptor);
#endif

#ifdef PHALANX_USING_FONTADAPTER_SDLTTF
	if (!fontAdapter)
		fontAdapter = new FontAdapter_SDL_ttf(this, fontDescriptor);
#endif

	if (fontAdapter)
		_fontAdapters[fontDescriptor] = fontAdapter;

	return fontAdapter;
}
