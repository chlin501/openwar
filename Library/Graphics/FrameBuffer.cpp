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
	std::swap(_status, rhs._status);
}


FrameBuffer& FrameBuffer::operator=(FrameBuffer&& rhs)
{
	std::swap(_id, rhs._id);
	std::swap(_status, rhs._status);
	return *this;
}


bool FrameBuffer::IsComplete() const
{
	return _status == GL_FRAMEBUFFER_COMPLETE;
}


const char* FrameBuffer::GetStatus() const
{
	switch (_status)
	{
		case GL_FRAMEBUFFER_COMPLETE: return "GL_FRAMEBUFFER_COMPLETE";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
#endif
		case GL_FRAMEBUFFER_UNSUPPORTED: return "GL_FRAMEBUFFER_UNSUPPORTED";
		default: return "glCheckFramebufferStatus=???";
	}
}


void FrameBuffer::AttachColor(RenderBuffer* value)
{
	GLint old;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	CHECK_ERROR_GL();

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, value->_id);
	CHECK_ERROR_GL();

	_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(old));
}


void FrameBuffer::AttachColor(Texture* value)
{
	GLint old;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	CHECK_ERROR_GL();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, value->_id, 0);
	CHECK_ERROR_GL();

	_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(old));
}


void FrameBuffer::AttachDepth(RenderBuffer* value)
{
	GLint old;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	CHECK_ERROR_GL();

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, value->_id);
	CHECK_ERROR_GL();

	_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(old));
}


void FrameBuffer::AttachDepth(Texture* value)
{
	GLint old;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	CHECK_ERROR_GL();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, value->_id, 0);
	CHECK_ERROR_GL();

	_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(old));
}


void FrameBuffer::AttachStencil(RenderBuffer* value)
{
	GLint old;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	CHECK_ERROR_GL();

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, value->_id);
	CHECK_ERROR_GL();

	_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(old));
}
