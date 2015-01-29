// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Texture_H
#define Texture_H

#include "GraphicsContext.h"

class GraphicsContext;


class Texture
{
public:
	GLuint _id;

	explicit Texture(GraphicsContext* gc);
	virtual ~Texture();

	Texture(Texture&&);
	Texture& operator=(Texture&&);

	virtual void UpdateTexture() = 0;
};


#endif
