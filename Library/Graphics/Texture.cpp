// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Texture.h"
#include "Image.h"


Texture::Texture(GraphicsContext*) :
	_id{}
{
	glGenTextures(1, &_id);
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



void Texture::LoadTextureFromImage(const Image& image)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixels());
	CHECK_ERROR_GL();
}


void Texture::LoadTextureFromData(int width, int height, const void* data)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	CHECK_ERROR_GL();
}


void Texture::GenerateMipmap()
{
	glBindTexture(GL_TEXTURE_2D, _id);
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();
}


void Texture::ResizeDepth(int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}
