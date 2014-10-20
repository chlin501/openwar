// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef FrameBuffer_H
#define FrameBuffer_H

#include "GraphicsOpenGL.h"

#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif

struct RenderBuffer;
struct texture;


struct FrameBuffer
{
	GLuint id;

	FrameBuffer();
	~FrameBuffer();

	void AttachColor(RenderBuffer* value);
	void AttachColor(texture* value);

	void AttachDepth(RenderBuffer* value);
	void AttachDepth(texture* value);

	void AttachStencil(RenderBuffer* value);

private:
	FrameBuffer(const FrameBuffer&) {}
	FrameBuffer& operator=(const FrameBuffer&) { return *this; }
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
