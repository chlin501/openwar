// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Texture.h"
#include "GraphicsContext.h"



Texture::Texture(GraphicsContext* gc) :
	_id{0}
{
	glGenTextures(1, &_id);
	CHECK_ERROR_GL();
	glBindTexture(GL_TEXTURE_2D, _id);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	CHECK_ERROR_GL();
}


Texture::~Texture()
{
	if (_id != 0)
	{
		glDeleteTextures(1, &_id);
		CHECK_ERROR_GL();
	}
}


Texture::Texture(Texture&& rhs)
{
	std::swap(_id, rhs._id);
}


Texture& Texture::operator=(Texture&& rhs)
{
	std::swap(_id, rhs._id);
	return *this;
}
