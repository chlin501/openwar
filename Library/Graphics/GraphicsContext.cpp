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
