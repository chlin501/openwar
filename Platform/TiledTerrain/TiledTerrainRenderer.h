// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TiledTerrainSurface_H
#define TiledTerrainSurface_H

class GraphicsContext;

#include <map>
#include "BattleModel/TiledGroundMap.h"
#include "Algebra/bounds.h"
#include "Graphics/Image.h"
#include "Algorithms/bspline_patch.h"
#include "Graphics/Texture.h"


class TiledTerrainRenderer
{
	GraphicsContext* _gc;
	TiledGroundMap* _tiledGroundMap;
	std::map<std::string, Texture*> _textures;

public:
	TiledTerrainRenderer(GraphicsContext* gc, TiledGroundMap* tiledGroundMap);
	~TiledTerrainRenderer();

	void Render(const glm::mat4x4& transform, const glm::vec3& lightNormal);
};


#endif
