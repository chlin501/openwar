// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ShaderProgram_H
#define ShaderProgram_H

#include "GraphicsContext.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include <map>
#include <vector>

#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif


#define VERTEX_SHADER(source) (#source)
#define FRAGMENT_SHADER(source) (#source)


class ShaderProgram
{
	friend class RenderCallBase;

	GLuint _program;
public:
	GLenum _blend_sfactor;
	GLenum _blend_dfactor;

public:
	ShaderProgram(const char* vertexshader, const char* fragmentshader);
	virtual ~ShaderProgram();

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

private:
	static GLuint CompileShader(GLenum type, const char* source);

	static bool LinkProgram(GLuint program);
	static bool ValidateProgram(GLuint program);
};


#endif
