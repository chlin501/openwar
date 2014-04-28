// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothGroundMap_H
#define SmoothGroundMap_H

#include <glm/glm.hpp>
#include <string>
#include "GroundMap.h"
#include "../Library/Algebra/bounds.h"
#include "../Library/Algebra/image.h"

class HeightMap;


class SmoothGroundMap : public GroundMap
{
	HeightMap* _heightMap;
	std::string _hash;
	bounds2f _bounds;
	image* _image;

public:
	SmoothGroundMap(HeightMap* heightMap, const char* hash, bounds2f bounds, image* img);

	const char* GetHash() const { return _hash.c_str(); }

	virtual bounds2f GetBounds() const { return _bounds; }
	virtual HeightMap* GetHeightMap() const { return _heightMap; }

	virtual bool IsForest(glm::vec2 position) const;
	virtual bool IsImpassable(glm::vec2 position) const;
	virtual bool IsWater(glm::vec2 position) const;
	virtual bool ContainsWater(bounds2f bounds) const;

	image* GetImage() const { return _image; }
	glm::ivec2 ToGroundmapCoordinate(glm::vec2 position) const;
	float CalculateHeight(int x, int y) const;

	float GetForestValue(int x, int y) const;
	float GetImpassableValue(int x, int y) const;

	void Extract(glm::vec2 position, image* brush);
	bounds2f Paint(TerrainFeature feature, glm::vec2 position, image* brush, float pressure);
	bounds2f Paint(TerrainFeature feature, glm::vec2 position, float radius, float pressure);

private:
	void UpdateHeightMap();
};


#endif
