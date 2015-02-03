// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Texture_H
#define Texture_H

#include "GraphicsContext.h"

class Image;


class Texture
{
	friend class RenderCallTexture;
	friend class FrameBuffer;

protected:
	GLuint _id;

public:
	explicit Texture(GraphicsContext* gc);
	virtual ~Texture();

	Texture(Texture&&);
	Texture& operator=(Texture&&);

	void Reset(GLenum internalFormat, GLenum type, GLsizei width, GLsizei height);

	virtual void UpdateTexture();

	void LoadTextureFromImage(const Image& image);
	void LoadTextureFromData(int width, int height, const void* data);

	void GenerateMipmap();
};


#endif
