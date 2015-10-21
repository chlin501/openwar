// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef OPENWARPLATFORM_H
#define OPENWARPLATFORM_H


/* ANDROID */

#if defined(OPENWAR_PLATFORM_ANDROID)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#define OPENWAR_USE_FONTADAPTER_SDLTTF
#define OPENWAR_USE_GLES2
#define OPENWAR_USE_SDL


/* LINUX */

#elif defined(OPENWAR_PLATFORM_LINUX)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <SDL2/SDL_image.h>
#define OPENWAR_USE_GLES2
#define OPENWAR_USE_SDL


/* IOS */

#elif defined(OPENWAR_PLATFORM_IOS)
#import <CoreGraphics/CoreGraphics.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#define OPENWAR_USE_AVFOUNDATION
#define OPENWAR_USE_AVAUDIOSESSION
#define OPENWAR_USE_AUDIOTOOLBOX
#define OPENWAR_USE_GLES2
#define OPENWAR_USE_OPENAL
#define OPENWAR_USE_UIKIT
#define OPENWAR_USE_SDL


/* MAC */

#elif defined(OPENWAR_PLATFORM_MAC)
#import <CoreGraphics/CoreGraphics.h>
#include <OpenGL/gl3.h>
#include <SDL2/SDL.h>
//#define glGenVertexArraysOES glGenVertexArraysAPPLE
//#define glBindVertexArrayOES glBindVertexArrayAPPLE
//#define glDeleteVertexArraysOES glDeleteVertexArraysAPPLE
#define OPENWAR_USE_APPKIT


/* WEB */

#elif defined(OPENWAR_PLATFORM_WEB)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#define OPENWAR_DISABLE_LEGACY_UI
#define OPENWAR_USE_GLES2
#define OPENWAR_USE_SDL


#else
#error "OPENWAR_PLATFORM_xxx not defined"
#endif


#ifndef GL_VERTEX_PROGRAM_POINT_SIZE
#define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
#endif

#ifndef GL_POINT_SPRITE
#define GL_POINT_SPRITE 0x8861
#endif


#endif
