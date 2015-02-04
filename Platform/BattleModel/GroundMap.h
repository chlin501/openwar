// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TerrainSurface_H
#define TerrainSurface_H

#include "Algebra/bounds.h"


enum class TerrainFeature { Hills, Water, Trees, Fords };

class HeightMap;


class GroundMap
{
public:
	virtual ~GroundMap();

	virtual bounds2f GetBounds() const = 0;
	virtual HeightMap* GetHeightMap() const = 0;

	virtual bool IsForest(glm::vec2 position) const = 0;
	virtual bool IsImpassable(glm::vec2 position) const = 0;
	virtual bool IsWater(glm::vec2 position) const = 0;
	virtual bool ContainsWater(bounds2f bounds) const = 0;
};


#endif
