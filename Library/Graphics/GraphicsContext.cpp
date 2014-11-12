// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "GraphicsContext.h"
#include "TextureAtlas.h"


GraphicsContext::GraphicsContext(float pixelDensity) :
	_pixeldensity(pixelDensity),
	_widgetTextureAtlas(nullptr)
{
}


GraphicsContext::~GraphicsContext()
{
	for (auto i : _fontAdapters)
		delete i.second;
}


float GraphicsContext::GetPixelDensity() const
{
	return _pixeldensity;
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

	FontAdapter* fontAdapter = new FontAdapter_NSFont(this, fontDescriptor);
	_fontAdapters[fontDescriptor] = fontAdapter;
	return fontAdapter;
}
