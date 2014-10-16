// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "RenderBufferX.h"


#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif



RenderBufferX::RenderBufferX(GLenum internalformat, GLsizei width, GLsizei height)
{
	glGenRenderbuffers(1, &id);
	CHECK_ERROR_GL();
	glBindRenderbuffer(GL_RENDERBUFFER, id);
	CHECK_ERROR_GL();
	glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
	CHECK_ERROR_GL();
}


void RenderBufferX::Resize(GLenum internalformat, GLsizei width, GLsizei height)
{
	glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
	CHECK_ERROR_GL();
}


RenderBufferX::~RenderBufferX()
{
	glDeleteRenderbuffers(1, &id);
	CHECK_ERROR_GL();
}
