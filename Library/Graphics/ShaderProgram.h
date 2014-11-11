// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ShaderProgram_H
#define ShaderProgram_H

#include "GraphicsOpenGL.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include <map>
#include <vector>

#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif


#define VERTEX_SHADER(source) (#source)
#define FRAGMENT_SHADER(source) (#source)


class ShaderProgramBase
{
	friend class RenderCallBase;

public:
	GLuint _program;
	GLenum _blend_sfactor;
	GLenum _blend_dfactor;

	ShaderProgramBase(const char* vertexshader, const char* fragmentshader);
	virtual ~ShaderProgramBase();

	static GLuint compile_shader(GLenum type, const char* source);

	static bool link_program(GLuint program);
	static bool validate_program(GLuint program);

protected:
	ShaderProgramBase(const ShaderProgramBase&) { }
	ShaderProgramBase& operator=(const ShaderProgramBase&) { return *this; }
};



template <class _Vertex>
class ShaderProgram : public ShaderProgramBase
{
public:
	typedef _Vertex VertexT;

	ShaderProgram(const char* vertexshader, const char* fragmentshader) : ShaderProgramBase(vertexshader, fragmentshader)
	{
	}

private:
	ShaderProgram(const ShaderProgram& _) : ShaderProgramBase(_) { }
	ShaderProgram& operator=(const ShaderProgram&) { return *this; }
};


#endif
