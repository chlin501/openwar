#ifndef SmoothGroundMap_H
#define SmoothGroundMap_H

#include <glm/glm.hpp>
#include "GroundMap.h"
#include "../Library/Algebra/bounds.h"
#include "../Library/Algebra/image.h"

class HeightMap;


class SmoothGroundMap : public GroundMap
{
	bounds2f _bounds;
	image* _image;
	HeightMap* _heightMap;

public:
	SmoothGroundMap(bounds2f bounds, image* img);

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
