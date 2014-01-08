// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothHeightMap_H
#define SmoothHeightMap_H

#include <functional>
#include <glm/glm.hpp>
#include "../Library/Algebra/geometry.h"


class HeightMap
{
	bounds2f _bounds;
	int _cacheStride;
	int _cacheMaxIndex;
	float* _cacheHeights;
	glm::vec3* _cacheNormals;

public:
	HeightMap(bounds2f bounds);
	~HeightMap();

	bounds2f GetBounds() const { return _bounds; }

	void Update(std::function<float(int, int)> calculateHeight);

	int GetMaxIndex() const { return _cacheMaxIndex; }
	float GetHeight(int x, int y) const;
	glm::vec3 GetNormal(int x, int y) const;

	float InterpolateHeight(glm::vec2 position) const;
	glm::vec3 GetPosition(glm::vec2 p, float h) { return glm::vec3(p, InterpolateHeight(p) + h); }

	const float* Intersect(ray r);

private:
	const float* InternalIntersect(ray r);

	void UpdateHeights(std::function<float(int, int)> calculateHeight);
	void UpdateNormals();
};


#endif
