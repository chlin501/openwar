#ifndef GraphicsOpenGL_H
#define GraphicsOpenGL_H


#ifdef GRAPHICS_OPENGL_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif


#ifdef GRAPHICS_OPENGL_MAC
#include <OpenGL/gl.h>
//#define glGenVertexArraysOES glGenVertexArraysAPPLE
//#define glBindVertexArrayOES glBindVertexArrayAPPLE
//#define glDeleteVertexArraysOES glDeleteVertexArraysAPPLE
#endif


#ifdef GRAPHICS_OPENGL_ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif


#ifdef GRAPHICS_OPENGL_LINUX
#endif


#ifndef CHECK_ERROR_GL
extern void CHECK_ERROR_GL();
#endif


#ifndef GL_POINT_SPRITE
#define GL_POINT_SPRITE 0x8861
#endif

/*
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#if TARGET_OS_IPHONE
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <OpenGL/gl.h>
#endif
#else
#if OPENWAR_USE_GLEW
#include <GL/glew.h>
#endif
#ifdef OPENWAR_USE_GLES2
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/gl.h>
#endif
#endif
*/


#endif
