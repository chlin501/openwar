// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "GraphicsContext.h"


int GraphicsContext::_shaderprogram_id = 0;


GraphicsContext::GraphicsContext(float pixelDensity) :
_pixeldensity(pixelDensity)
{
}


GraphicsContext::~GraphicsContext()
{
}


int GraphicsContext::generate_shaderprogram_id()
{
	return ++_shaderprogram_id;
}
