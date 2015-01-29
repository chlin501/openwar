// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RenderBuffer_H
#define RenderBuffer_H

#include "GraphicsContext.h"


struct RenderBuffer
{
	GLuint id;

	RenderBuffer(GLenum internalformat, GLsizei width, GLsizei height);
	~RenderBuffer();

	RenderBuffer(const RenderBuffer&) = delete;
	RenderBuffer& operator=(const RenderBuffer&) = delete;

	void Resize(GLenum internalformat, GLsizei width, GLsizei height);
};


#endif
