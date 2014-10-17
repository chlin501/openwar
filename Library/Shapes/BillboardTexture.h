// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BILLBOARDTEXTURE_H
#define BILLBOARDTEXTURE_H

#include <map>
#include <vector>

#include "Algebra/affine2.h"
#include "imagex.h"
#include "Graphics/texture.h"


class BillboardTexture
{
	struct item
	{
		int shape;
		float facing; // degrees
		affine2 texcoords;
		item(int s, float f, affine2 t) : shape(s), facing(f), texcoords(t) { }
	};

	texture* _texture;
    std::map<int, std::vector<item>> _items;
	int _shapeCount;

public:
	BillboardTexture();
	~BillboardTexture();

	texture* GetTexture() const { return _texture; }

	int AddSheet(const imagex& img);
	int AddShape(int sheet);

	void SetTexCoords(int shape, float facing, const affine2& texcoords);

	affine2 GetTexCoords(int shape, float facing);
};


#endif
