// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
#import <Foundation/Foundation.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif
#endif

#include "Texture.h"
#include "ShaderProgram.h"
#include "Image.h"
#include "GraphicsContext.h"



Texture::Texture(GraphicsContext* gc)
{
	glGenTextures(1, &id);
	CHECK_ERROR_GL();
	glBindTexture(GL_TEXTURE_2D, id);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_CLAMP_TO_EDGE
	CHECK_ERROR_GL();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_CLAMP_TO_EDGE
	CHECK_ERROR_GL();}


Texture::~Texture()
{
	glDeleteTextures(1, &id);
	CHECK_ERROR_GL();
}
