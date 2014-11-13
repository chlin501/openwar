// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TextureResource_H
#define TextureResource_H

#include "Texture.h"
#include "resource.h"


class TextureResource : public Texture
{
public:
	TextureResource(GraphicsContext* gc, const resource& r);
	void LoadTextureFromResource(GraphicsContext* gc, const resource& r);

	virtual void UpdateTexture();
};


#endif
