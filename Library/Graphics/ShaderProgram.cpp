// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ShaderProgram.h"
#include <cstdlib>
#include <string>

#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif
#endif


#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
#import <Foundation/Foundation.h>
#endif

//#include <android/log.h>
//#import <Foundation/Foundation.h>


#ifndef CHECK_ERROR_GL
void CHECK_ERROR_GL()
{
	//GLenum error = glGetError();
	//if (error != GL_NO_ERROR)
	//	NSLog(@"glGetError() = %#06x", (int)error); //TODO: should log error
}
#endif


static void print_log(const char* operation, const char* log)
{
//	__android_log_print(ANDROID_LOG_INFO, "OPENWAR", "%s: %s", operation, log);
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
	NSLog(@"RENDERER log (%s):\n%s", operation, log);
#endif
}


ShaderProgram::ShaderProgram(const char* vertexshader, const char* fragmentshader) :
_blend_sfactor(GL_ONE),
_blend_dfactor(GL_ZERO)
{
	_program = glCreateProgram();
	CHECK_ERROR_GL();

	GLuint vertex_shader = CompileShader(GL_VERTEX_SHADER, vertexshader);
	GLuint fragment_shader = CompileShader(GL_FRAGMENT_SHADER, fragmentshader);

    glAttachShader(_program, vertex_shader);
	CHECK_ERROR_GL();
    glAttachShader(_program, fragment_shader);
	CHECK_ERROR_GL();

    if (!LinkProgram(_program)) {
        if (_program) {
            glDeleteProgram(_program);
	        CHECK_ERROR_GL();
	        _program = 0;
        }
        return;
    }
	ValidateProgram(_program);

	glDetachShader(_program, vertex_shader);
	CHECK_ERROR_GL();
	glDetachShader(_program, fragment_shader);
	CHECK_ERROR_GL();

	glDeleteShader(vertex_shader);
	CHECK_ERROR_GL();
	glDeleteShader(fragment_shader);
	CHECK_ERROR_GL();
}


ShaderProgram::~ShaderProgram()
{
    if (_program != 0)
    {
        glDeleteProgram(_program);
	    CHECK_ERROR_GL();
    }
}


GLuint ShaderProgram::CompileShader(GLenum type, const char* source)
{
    std::string str(source);
    
    if (str.size() >= 2 && str[0] == '{' && str[str.size() - 1] == '}')
        str = str.substr(1, str.size() - 2);
    
#ifdef OPENWAR_USING_GLES2
    str.insert(0, "precision highp float; precision lowp int; ");
#else
    str.insert(0, "#version 120\n");
#endif

	const GLchar *src = str.c_str();

	GLuint result = glCreateShader(type);
	CHECK_ERROR_GL();
	glShaderSource(result, 1, &src, NULL);
	CHECK_ERROR_GL();
	glCompileShader(result);
	CHECK_ERROR_GL();

	#if 1 //defined(DEBUG)
	GLint logLength;
	glGetShaderiv(result, GL_INFO_LOG_LENGTH, &logLength);
	CHECK_ERROR_GL();
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)std::malloc((size_t)logLength);
		glGetShaderInfoLog(result, logLength, &logLength, log);
		CHECK_ERROR_GL();
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
	CHECK_ERROR_GL();

#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	CHECK_ERROR_GL();
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc((size_t)logLength);
        glGetProgramInfoLog(program, logLength, &logLength, log);
	    CHECK_ERROR_GL();
		print_log("log", log);
        free(log);
    }
#endif

    glGetProgramiv(program, GL_LINK_STATUS, &status);
	CHECK_ERROR_GL();
    if (status == 0) {
        return false;
    }

    return true;
}


bool ShaderProgram::ValidateProgram(GLuint program)
{
	GLint logLength, status;

	glValidateProgram(program);
	CHECK_ERROR_GL();
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	CHECK_ERROR_GL();
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)std::malloc((size_t)logLength);
		glGetProgramInfoLog(program, logLength, &logLength, log);
		print_log("validate", log);
		std::free(log);
	}

	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	CHECK_ERROR_GL();
	if (status == 0) {
		return false;
	}

	return true;
}
