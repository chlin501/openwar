// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RenderBuffer_H
#define RenderBuffer_H

#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#if TARGET_OS_IPHONE
#include <OpenGLES/ES2/gl.h>
#else
#include <OpenGL/gl.h>
#endif
#else
#if OPENWAR_USE_GLEW
#include <GL/glew.h>
#endif
#ifdef OPENWAR_USE_GLES2
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#endif
#endif


struct RenderBuffer
{
	GLuint id;

	RenderBuffer(GLenum internalformat, GLsizei width, GLsizei height);
	~RenderBuffer();

	void Resize(GLenum internalformat, GLsizei width, GLsizei height);

private:
	RenderBuffer(const RenderBuffer&) {}
	RenderBuffer& operator=(const RenderBuffer&) { return *this; }
};


#endif
