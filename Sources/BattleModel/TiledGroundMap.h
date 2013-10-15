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
		std::string texture;
		int rotate; // counterclockwise
		bool mirror;

		Tile() : texture(), rotate(0), mirror(false) { }
	};

private:
	bounds2f _bounds;
	glm::ivec2 _size;
	HeightMap* _heightMap;
	Tile* _tiles;
	bspline_patch* _patch;

public:
	TiledGroundMap(bounds2f bounds, glm::ivec2 size);
	virtual ~TiledGroundMap();

	// GroundMap

	virtual bounds2f GetBounds() const;
	virtual HeightMap* GetHeightMap() const;

	virtual bool IsForest(glm::vec2 position) const;
	virtual bool IsImpassable(glm::vec2 position) const;
	virtual bool IsWater(glm::vec2 position) const;
	virtual bool ContainsWater(bounds2f bounds) const;

	// TiledGroundMap

	glm::ivec2 GetSize() const { return _size; }

	void SetHeight(int x, int y, float h);
	float GetHeight(int x, int y) const;

	void UpdateHeightMap();

	void SetTile(int x, int y, const std::string& texture, int rotate, bool mirror);
	Tile* GetTile(int x, int y);
};


#endif
