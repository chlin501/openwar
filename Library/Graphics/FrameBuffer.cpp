// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "FrameBuffer.h"
#include "RenderBuffer.h"
#include "Texture.h"


FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &_id);
	CHECK_ERROR_GL();
}


FrameBuffer::~FrameBuffer()
{
	if (_id != 0)
	{
		glDeleteFramebuffers(1, &_id);
		CHECK_ERROR_GL();
	}
}


FrameBuffer::FrameBuffer(FrameBuffer&& rhs)
{
	std::swap(_id, rhs._id);
	std::swap(_complete, rhs._complete);
}


FrameBuffer& FrameBuffer::operator=(FrameBuffer&& rhs)
{
	std::swap(_id, rhs._id);
	std::swap(_complete, rhs._complete);
	return *this;
}


bool FrameBuffer::IsComplete() const
{
	return _complete;
}


void FrameBuffer::AttachColor(RenderBuffer* value)
{
	GLint old;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, value->_id);
	CHECK_ERROR_GL();

	_complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(old));
}


void FrameBuffer::AttachColor(Texture* value)
{
	GLint old;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, value->_id, 0);
	CHECK_ERROR_GL();

	_complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(old));
}


void FrameBuffer::AttachDepth(RenderBuffer* value)
{
	GLint old;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, value->_id);
	CHECK_ERROR_GL();

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(old));
}


void FrameBuffer::AttachDepth(Texture* value)
{
	GLint old;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, value->_id, 0);
	CHECK_ERROR_GL();

	_complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(old));
}


void FrameBuffer::AttachStencil(RenderBuffer* value)
{
	GLint old;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, value->_id);
	CHECK_ERROR_GL();

	_complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(old));
}
