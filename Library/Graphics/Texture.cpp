// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Texture.h"
#include "Image.h"


Texture::Texture(GraphicsContext*) :
	_id{}
{
	glGenTextures(1, &_id);
    CHECK_OPENGL_ERROR();
}


Texture::~Texture()
{
	if (_id != 0)
	{
		glDeleteTextures(1, &_id);
        CHECK_OPENGL_ERROR();
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


void Texture::PrepareColorBuffer(GLsizei width, GLsizei height)
{
	glBindTexture(GL_TEXTURE_2D, _id);
    CHECK_OPENGL_ERROR();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    CHECK_OPENGL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECK_OPENGL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECK_OPENGL_ERROR();
	glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_OPENGL_ERROR();
}


void Texture::PrepareDepthBuffer(GLsizei width, GLsizei height)
{
	glBindTexture(GL_TEXTURE_2D, _id);
    CHECK_OPENGL_ERROR();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
    CHECK_OPENGL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECK_OPENGL_ERROR();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECK_OPENGL_ERROR();
	glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_OPENGL_ERROR();
}


void Texture::UpdateTexture()
{
}


void Texture::LoadTextureFromImage(const Image& image)
{
	glBindTexture(GL_TEXTURE_2D, _id);
    CHECK_OPENGL_ERROR();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixels());
    CHECK_OPENGL_ERROR();
}


void Texture::LoadTextureFromData(int width, int height, const void* data)
{
	glBindTexture(GL_TEXTURE_2D, _id);
    CHECK_OPENGL_ERROR();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    CHECK_OPENGL_ERROR();
}


void Texture::GenerateMipmap()
{
	glBindTexture(GL_TEXTURE_2D, _id);
    CHECK_OPENGL_ERROR();
	glGenerateMipmap(GL_TEXTURE_2D);
    CHECK_OPENGL_ERROR();
}
