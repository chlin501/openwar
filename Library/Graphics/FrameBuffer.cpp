// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "FrameBuffer.h"
#include "RenderBuffer.h"
#include "texturex.h"


FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &id);
	CHECK_ERROR_GL();
}


FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &id);
	CHECK_ERROR_GL();
}


void FrameBuffer::AttachColor(RenderBuffer* value)
{
	bind_framebuffer binding(*this);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, value->id);
	CHECK_ERROR_GL();
}


void FrameBuffer::AttachColor(texturex* value)
{
	bind_framebuffer binding(*this);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, value->id, 0);
	CHECK_ERROR_GL();
}


void FrameBuffer::AttachDepth(RenderBuffer* value)
{
	bind_framebuffer binding(*this);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, value->id);
	CHECK_ERROR_GL();
}


void FrameBuffer::AttachDepth(texturex* value)
{
	bind_framebuffer binding(*this);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, value->id, 0);
	CHECK_ERROR_GL();
}


void FrameBuffer::AttachStencil(RenderBuffer* value)
{
	bind_framebuffer binding(*this);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, value->id);
	CHECK_ERROR_GL();
}
