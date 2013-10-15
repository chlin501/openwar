// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TiledTerrainSurface_H
#define TiledTerrainSurface_H

#include <map>
#include "../../BattleModel/TiledGroundMap.h"
#include "../../Library/Algebra/bounds.h"
#include "../../Library/Algebra/image.h"
#include "../../Library/Algorithms/bspline_patch.h"
#include "../../Library/Graphics/texture.h"


class TiledTerrainRenderer
{

private:
	TiledGroundMap* _tiledGroundMap;
	std::map<std::string, texture*> _textures;

public:
	TiledTerrainRenderer(TiledGroundMap* tiledGroundMap);
	~TiledTerrainRenderer();

	void Render(const glm::mat4x4& transform, const glm::vec3& lightNormal);
};


#endif
