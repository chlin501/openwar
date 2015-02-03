// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "RenderBuffer.h"


#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif



RenderBuffer::RenderBuffer()
{
	glGenRenderbuffers(1, &_id);
	CHECK_ERROR_GL();
}


RenderBuffer::~RenderBuffer()
{
	glDeleteRenderbuffers(1, &_id);
	CHECK_ERROR_GL();
}


void RenderBuffer::Reset(GLenum internalFormat, GLsizei width, GLsizei height)
{
	glBindRenderbuffer(GL_RENDERBUFFER, _id);
	CHECK_ERROR_GL();
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
	CHECK_ERROR_GL();
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	CHECK_ERROR_GL();
}
