// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RenderBuffer_H
#define RenderBuffer_H

#include "GraphicsContext.h"


class RenderBuffer
{
	friend class FrameBuffer;
	GLuint _id{};

public:
	RenderBuffer();
	~RenderBuffer();

	RenderBuffer(const RenderBuffer&) = delete;
	RenderBuffer& operator=(const RenderBuffer&) = delete;

	void PrepareColorBuffer(GLsizei width, GLsizei height);
	void PrepareDepthBuffer(GLsizei width, GLsizei height);
};


#endif
