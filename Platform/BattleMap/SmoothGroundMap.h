// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothGroundMap_H
#define SmoothGroundMap_H

#include <glm/glm.hpp>
#include <string>
#include "GroundMap.h"
#include "Algebra/bounds.h"
#include "Graphics/Image.h"
#include "HeightMap.h"


class SmoothGroundMap : public GroundMap
{
	bounds2f _bounds;
	std::unique_ptr<Image> _image;

public:
	HeightMap _heightMap;

	SmoothGroundMap(bounds2f bounds, std::unique_ptr<Image>&& image);
	~SmoothGroundMap();

public: //GroundMap
	bounds2f GetBounds() const override { return _bounds; }
	const HeightMap* GetHeightMap() const override { return &_heightMap; }
	float CalculateHeight(int x, int y) const;

	bool IsForest(glm::vec2 position) const override;
	bool IsImpassable(glm::vec2 position) const override;
	bool IsWater(glm::vec2 position) const override;
	bool ContainsWater(bounds2f bounds) const override;

public:
	Image* GetImage() const { return _image.get(); }
	glm::ivec2 ToGroundmapCoordinate(glm::vec2 position) const;

	float GetForestValue(int x, int y) const;
	float GetImpassableValue(int x, int y) const;

	void Extract(glm::vec2 position, Image* brush);
	bounds2f Paint(TerrainFeature feature, glm::vec2 position, Image* brush, float pressure);
	bounds2f Paint(TerrainFeature feature, glm::vec2 position, float radius, float pressure);

private:
	void UpdateHeightMap();
};


#endif
