// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef GroundMap_H
#define GroundMap_H

#include "Algebra/bounds.h"
#include "HeightMap.h"

class HeightMap;


class GroundMap
{
public:
	virtual ~GroundMap();

	virtual bounds2f GetBounds() const = 0;
	virtual const HeightMap* GetHeightMap() const = 0;
	virtual float CalculateHeight(int x, int y) const = 0;

	virtual bool IsForest(glm::vec2 position) const = 0;
	virtual bool IsImpassable(glm::vec2 position) const = 0;
	//virtual bool IsWater(glm::vec2 position) const = 0;
	virtual bool ContainsWater(bounds2f bounds) const = 0;
};


class BlankGroundMap : public GroundMap
{
	HeightMap _heightMap{{0, 0, 1024, 1024}};

public:
	bounds2f GetBounds() const { return _heightMap.GetBounds(); }
	const HeightMap* GetHeightMap() const { return &_heightMap; }
	float CalculateHeight(int x, int y) const { return 2.0f; }

	bool IsForest(glm::vec2 position) const { return false; }
	bool IsImpassable(glm::vec2 position) const { return false; }
	bool ContainsWater(bounds2f bounds) const { return false; }
};


#endif
