// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "RenderBuffer.h"



RenderBuffer::RenderBuffer()
{
	glGenRenderbuffers(1, &_id);
    CHECK_OPENGL_ERROR();
}


RenderBuffer::~RenderBuffer()
{
	glDeleteRenderbuffers(1, &_id);
    CHECK_OPENGL_ERROR();
}


void RenderBuffer::PrepareColorBuffer(GLsizei width, GLsizei height)
{
	glBindRenderbuffer(GL_RENDERBUFFER, _id);
    CHECK_OPENGL_ERROR();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, width, height);
    CHECK_OPENGL_ERROR();
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
    CHECK_OPENGL_ERROR();
}


void RenderBuffer::PrepareDepthBuffer(GLsizei width, GLsizei height)
{
	glBindRenderbuffer(GL_RENDERBUFFER, _id);
    CHECK_OPENGL_ERROR();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    CHECK_OPENGL_ERROR();
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
    CHECK_OPENGL_ERROR();
}
