// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TiledGroundMap.h"
#include "HeightMap.h"
#include "Algorithms/bspline_patch.h"



TiledGroundMap::TiledGroundMap(bounds2f bounds, glm::ivec2 size) :
_heightMap(nullptr),
_bounds(bounds),
_size(size)
{
	_heightMap = new HeightMap(bounds2f(0, 0, 1024, 1024));
	_tiles = new Tile[size.x * size.y];
	_patch = new bspline_patch(glm::ivec2(size.x + 1, size.y + 1));
}


TiledGroundMap::~TiledGroundMap()
{
	delete _heightMap;
	delete [] _tiles;
	delete _patch;
}


/* GroundMap */


bounds2f TiledGroundMap::GetBounds() const
{
	return _bounds;
}


const HeightMap* TiledGroundMap::GetHeightMap() const
{
	return _heightMap;
}


float TiledGroundMap::CalculateHeight(int x, int y) const
{
	return _patch->get_height(x, y);
}


bool TiledGroundMap::IsForest(glm::vec2 position) const
{
	return false;
}


bool TiledGroundMap::IsImpassable(glm::vec2 position) const
{
	return false;
}


bool TiledGroundMap::IsWater(glm::vec2 position) const
{
	return false;
}


bool TiledGroundMap::ContainsWater(bounds2f bounds) const
{
	return false;
}


/* TiledGroundMap */



void TiledGroundMap::SetHeight(int x, int y, float h)
{
	_patch->set_height(x, y, h);
}


void TiledGroundMap::UpdateHeightMap()
{
	_heightMap->Update(this);
}


void TiledGroundMap::SetTile(int x, int y, const std::string& texture, int rotate, bool mirror)
{
	Tile* tile = GetTile(x, y);
	tile->texture = texture;
	tile->rotate = rotate;
	tile->mirror = mirror;
}


TiledGroundMap::Tile* TiledGroundMap::GetTile(int x, int y) const
{
	if (0 <= x && x < _size.x && 0 <= y && y < _size.y)
		return _tiles + x + _size.x * y;

	return nullptr;
}
