// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TiledGroundMap_H
#define TiledGroundMap_H

#include <string>
#include "GroundMap.h"

class bspline_patch;


class TiledGroundMap : public GroundMap
{
public:
	struct Tile
	{
		std::string texture{};
		int rotate{}; // counterclockwise
		bool mirror{};
	};

private:
	HeightMap* _heightMap;
	bounds2f _bounds;
	glm::ivec2 _size;
	Tile* _tiles{};
	bspline_patch* _patch{};

public:
	TiledGroundMap(HeightMap* heightMap, bounds2f bounds, glm::ivec2 size);
	virtual ~TiledGroundMap();

public: // GroundMap
	bounds2f GetBounds() const override;
	HeightMap* GetHeightMap() const override;
	float CalculateHeight(int x, int y) const override;

	bool IsForest(glm::vec2 position) const override;
	bool IsImpassable(glm::vec2 position) const override;
	bool IsWater(glm::vec2 position) const override;
	bool ContainsWater(bounds2f bounds) const override;

public: // TiledGroundMap
	glm::ivec2 GetSize() const { return _size; }

	void SetHeight(int x, int y, float h);

	void UpdateHeightMap();

	void SetTile(int x, int y, const std::string& texture, int rotate, bool mirror);
	Tile* GetTile(int x, int y);
};


#endif
