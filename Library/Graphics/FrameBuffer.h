// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef FrameBuffer_H
#define FrameBuffer_H

#include "GraphicsContext.h"

#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif

struct RenderBuffer;
struct Texture;


struct FrameBuffer
{
	friend class RenderCallBase;

	GLuint id;

public:
	FrameBuffer();
	~FrameBuffer();

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;

	void AttachColor(RenderBuffer* value);
	void AttachColor(Texture* value);

	void AttachDepth(RenderBuffer* value);
	void AttachDepth(Texture* value);

	void AttachStencil(RenderBuffer* value);
};


class bind_framebuffer
{
	GLint _old;

public:
	bind_framebuffer(const FrameBuffer& fb)
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_old);
		glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
		CHECK_ERROR_GL();
	}

	~bind_framebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _old);
	}
};


#endif
