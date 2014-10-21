// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "VertexBuffer.h"
#include "ShaderProgram.h"


VertexBufferBase::VertexBufferBase() :
_mode(0),
_vbo(0),
_count(0)
{
}




VertexBufferBase::~VertexBufferBase()
{
	if (_vbo != 0)
	{
		glDeleteBuffers(1, &_vbo);
		CHECK_ERROR_GL();
	}
}
