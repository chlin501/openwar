// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Texture.h"
#include "GraphicsContext.h"



Texture::Texture(GraphicsContext* gc)
{
	glGenTextures(1, &_id);
	CHECK_ERROR_GL();
	glBindTexture(GL_TEXTURE_2D, _id);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_CLAMP_TO_EDGE
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_CLAMP_TO_EDGE
	CHECK_ERROR_GL();}


Texture::~Texture()
{
	glDeleteTextures(1, &_id);
	CHECK_ERROR_GL();
}
