// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "VertexBuffer.h"
#include "ShaderProgram.h"


VertexBufferBase::VertexBufferBase()
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


VertexBufferBase::VertexBufferBase(VertexBufferBase&& rhs)
{
	std::swap(_mode, rhs._mode);
	std::swap(_vbo, rhs._vbo);
	std::swap(_count, rhs._count);
}


VertexBufferBase& VertexBufferBase::operator=(VertexBufferBase&& rhs)
{
	std::swap(_mode, rhs._mode);
	std::swap(_vbo, rhs._vbo);
	std::swap(_count, rhs._count);
	return *this;
}
