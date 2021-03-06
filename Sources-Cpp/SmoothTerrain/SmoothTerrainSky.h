// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainSky_H
#define SmoothTerrainSky_H

#include "TerrainSky/TerrainSky.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/CommonShaders.h"


class SmoothTerrainSky : public TerrainSky
{
	GraphicsContext* _gc;
	Texture* _textureBackgroundLinen;

public:
	SmoothTerrainSky(GraphicsContext* gc);
	virtual ~SmoothTerrainSky();

	void Render(Viewport* viewport, float cameraDirectionZ);
	void RenderBackgroundLinen(Viewport* viewport, const bounds2f& bounds);
};



#endif
