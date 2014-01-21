// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "graphicscontext.h"


int graphicscontext::_shaderprogram_id = 0;


graphicscontext::graphicscontext(float pixelDensity) :
_pixeldensity(pixelDensity)
{
}


graphicscontext::~graphicscontext()
{
}


int graphicscontext::generate_shaderprogram_id()
{
	return ++_shaderprogram_id;
}
