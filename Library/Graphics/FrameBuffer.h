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


class FrameBuffer
{
	friend class RenderCallBase;

	GLuint _id;
	bool _complete{};

public:
	FrameBuffer();
	~FrameBuffer();

	FrameBuffer(FrameBuffer&&);
	FrameBuffer& operator=(FrameBuffer&&);

	bool IsComplete() const;

	void AttachColor(RenderBuffer* value);
	void AttachColor(Texture* value);

	void AttachDepth(RenderBuffer* value);
	void AttachDepth(Texture* value);

	void AttachStencil(RenderBuffer* value);
};


#endif
