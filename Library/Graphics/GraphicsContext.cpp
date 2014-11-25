// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "GraphicsContext.h"
#include "TextureAtlas.h"


GraphicsContext::GraphicsContext(float nativeScaling, float virtualScaling) :
	_nativeScaling(nativeScaling),
	_virtualScaling(virtualScaling),
	_widgetTextureAtlas(nullptr)
{
}


GraphicsContext::~GraphicsContext()
{
	for (auto i : _fontAdapters)
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


TextureAtlas* GraphicsContext::GetWidgetTextureAtlas() const
{
	if (_widgetTextureAtlas == nullptr)
		_widgetTextureAtlas = new TextureAtlas(const_cast<GraphicsContext*>(this));

	return _widgetTextureAtlas;
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
