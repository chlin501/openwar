// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "GraphicsContext.h"
#include "TextureAtlas.h"
#include "ShaderProgram.h"
#include "FontAdapter.h"


GraphicsContext::GraphicsContext(float nativeScaling, float virtualScaling) :
	_nativeScaling(nativeScaling),
	_virtualScaling(virtualScaling)
{
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);

/*#if !defined(OPENWAR_USE_GLES2)
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);
#endif*/
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

#ifdef ENABLE_FONTADAPTER_NSFONT
	if (fontAdapter == nullptr)
		fontAdapter = new FontAdapter_NSFont(this, fontDescriptor);
#endif

#ifdef ENABLE_FONTADAPTER_UIFONT
	if (fontAdapter == nullptr)
		fontAdapter = new FontAdapter_UIFont(this, fontDescriptor);
#endif

#ifdef ENABLE_FONTADAPTER_SDL_TTF
	if (fontAdapter == nullptr)
		fontAdapter = new FontAdapter_SDL_ttf(this, fontDescriptor);
#endif

	if (fontAdapter != nullptr)
		_fontAdapters[fontDescriptor] = fontAdapter;

	return fontAdapter;
}
