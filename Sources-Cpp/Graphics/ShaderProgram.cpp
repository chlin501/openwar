// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ShaderProgram.h"
#include <cstdlib>
#include <string>

#if defined(OPENWAR_PLATFORM_IOS) || defined(OPENWAR_PLATFORM_MAC)
#ifdef OPENWAR_PLATFORM_IOS
#import <UIKit/UIKit.h>
#endif
#endif


#if defined(OPENWAR_PLATFORM_IOS) || defined(OPENWAR_PLATFORM_MAC)
#import <Foundation/Foundation.h>
#endif

//#include <android/log.h>
//#import <Foundation/Foundation.h>




static void print_log(const char* operation, const char* log)
{
//	__android_log_print(ANDROID_LOG_INFO, "OPENWAR", "%s: %s", operation, log);
#if defined(OPENWAR_PLATFORM_IOS) || defined(OPENWAR_PLATFORM_MAC)
	NSLog(@"RENDERER log (%s):\n%s", operation, log);
#endif
}


ShaderProgram::ShaderProgram(const char* vertexshader, const char* fragmentshader) :
_blend_sfactor(GL_ONE),
_blend_dfactor(GL_ZERO)
{
	_program = glCreateProgram();
    CHECK_OPENGL_ERROR();

	GLuint vertex_shader = CompileShader(GL_VERTEX_SHADER, vertexshader);
	GLuint fragment_shader = CompileShader(GL_FRAGMENT_SHADER, fragmentshader);

    glAttachShader(_program, vertex_shader);
    CHECK_OPENGL_ERROR();
    glAttachShader(_program, fragment_shader);
    CHECK_OPENGL_ERROR();

    if (!LinkProgram(_program)) {
        if (_program) {
            glDeleteProgram(_program);
            CHECK_OPENGL_ERROR();
	        _program = 0;
        }
        return;
    }
	ValidateProgram(_program);

	glDetachShader(_program, vertex_shader);
    CHECK_OPENGL_ERROR();
	glDetachShader(_program, fragment_shader);
    CHECK_OPENGL_ERROR();

	glDeleteShader(vertex_shader);
    CHECK_OPENGL_ERROR();
	glDeleteShader(fragment_shader);
    CHECK_OPENGL_ERROR();
}


ShaderProgram::~ShaderProgram()
{
    if (_program != 0)
    {
        glDeleteProgram(_program);
        CHECK_OPENGL_ERROR();
    }
}


GLuint ShaderProgram::CompileShader(GLenum type, const char* source)
{
    std::string str(source);
    
    if (str.size() >= 2 && str[0] == '{' && str[str.size() - 1] == '}')
        str = str.substr(1, str.size() - 2);
    
#ifdef OPENWAR_USE_GLES2
    str.insert(0, "precision highp float; precision lowp int; ");
#else
    str.insert(0, "#version 120\n");
#endif

	const GLchar *src = str.c_str();

	GLuint result = glCreateShader(type);
    CHECK_OPENGL_ERROR();
	glShaderSource(result, 1, &src, NULL);
    CHECK_OPENGL_ERROR();
	glCompileShader(result);
    CHECK_OPENGL_ERROR();

	#if 1 //defined(DEBUG)
	GLint logLength;
	glGetShaderiv(result, GL_INFO_LOG_LENGTH, &logLength);
    CHECK_OPENGL_ERROR();
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)std::malloc((size_t)logLength);
		glGetShaderInfoLog(result, logLength, &logLength, log);
        CHECK_OPENGL_ERROR();
		print_log("compile", log);
		std::free(log);
	}
	#endif

	//GLint status;
	//glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

	return result;
}


bool ShaderProgram::LinkProgram(GLuint program)
{
    GLint status;
    glLinkProgram(program);
    CHECK_OPENGL_ERROR();

#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	CHECK_OPENGL_ERROR();
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc((size_t)logLength);
        glGetProgramInfoLog(program, logLength, &logLength, log);
	    CHECK_OPENGL_ERROR();
		print_log("log", log);
        free(log);
    }
#endif

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    CHECK_OPENGL_ERROR();
    if (status == 0) {
        return false;
    }

    return true;
}


bool ShaderProgram::ValidateProgram(GLuint program)
{
	GLint logLength, status;

	glValidateProgram(program);
    CHECK_OPENGL_ERROR();
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    CHECK_OPENGL_ERROR();
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)std::malloc((size_t)logLength);
		glGetProgramInfoLog(program, logLength, &logLength, log);
		print_log("validate", log);
		std::free(log);
	}

	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    CHECK_OPENGL_ERROR();
	if (status == 0) {
		return false;
	}

	return true;
}
