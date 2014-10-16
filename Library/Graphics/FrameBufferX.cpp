// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "FrameBufferX.h"
#include "RenderBufferX.h"
#include "texture.h"


FrameBufferX::FrameBufferX()
{
	glGenFramebuffers(1, &id);
	CHECK_ERROR_GL();
}


FrameBufferX::~FrameBufferX()
{
	glDeleteFramebuffers(1, &id);
	CHECK_ERROR_GL();
}


void FrameBufferX::AttachColor(RenderBufferX* value)
{
	bind_framebuffer binding(*this);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, value->id);
	CHECK_ERROR_GL();
}


void FrameBufferX::AttachColor(texture* value)
{
	bind_framebuffer binding(*this);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, value->id, 0);
	CHECK_ERROR_GL();
}


void FrameBufferX::AttachDepth(RenderBufferX* value)
{
	bind_framebuffer binding(*this);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, value->id);
	CHECK_ERROR_GL();
}


void FrameBufferX::AttachDepth(texture* value)
{
	bind_framebuffer binding(*this);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, value->id, 0);
	CHECK_ERROR_GL();
}


void FrameBufferX::AttachStencil(RenderBufferX* value)
{
	bind_framebuffer binding(*this);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, value->id);
	CHECK_ERROR_GL();
}
