// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <algorithm>
#include "HeightMap.h"



HeightMap::HeightMap(bounds2f bounds) :
_bounds(bounds),
_cacheStride(256),
_cacheMaxIndex(254),
_cacheHeights(nullptr),
_cacheNormals(nullptr)
{
	int n = _cacheStride * _cacheStride;
	_cacheHeights = new float [n];
	_cacheNormals = new glm::vec3[n];
	std::fill(_cacheHeights, _cacheHeights + n, 0.0f);
	std::fill(_cacheNormals, _cacheNormals + n, glm::vec3(0, 0, 1));
}


HeightMap::~HeightMap()
{
	delete _cacheHeights;
	delete _cacheNormals;

}


void HeightMap::Update(std::function<float(int, int)> calculateHeight)
{
	UpdateHeights(calculateHeight);
	UpdateNormals();
}


static float nearest_odd(float value)
{
	return 1.0f + 2.0f * (int)glm::round(0.5f * (value - 1.0f));
}


float HeightMap::GetHeight(int x, int y) const
{
	if (x < 0) x = 0; else if (x > _cacheMaxIndex) x = _cacheMaxIndex;
	if (y < 0) y = 0; else if (y > _cacheMaxIndex) y = _cacheMaxIndex;
	return _cacheHeights[x + y * _cacheStride];
}


glm::vec3 HeightMap::GetNormal(int x, int y) const
{
	if (x < 0) x = 0; else if (x > _cacheMaxIndex) x = _cacheMaxIndex;
	if (y < 0) y = 0; else if (y > _cacheMaxIndex) y = _cacheMaxIndex;
	return _cacheNormals[x + y * _cacheStride];
}


float HeightMap::InterpolateHeight(glm::vec2 position) const
{
	glm::vec2 p = (position - _bounds.min) / _bounds.size();

	float x = p.x * _cacheStride;
	float y = p.y * _cacheStride;

	// find triangle midpoint coordinates (x1, y1)

	float x1 = nearest_odd(x);
	float y1 = nearest_odd(y);

	// find triangle {(x1, y1), (x2, y2), (x3, y3)} containing (x, y)

	float sx2, sx3, sy2, sy3;
	float dx = x - x1;
	float dy = y - y1;
	if (glm::abs(dx) > glm::abs(dy))
	{
		sx2 = sx3 = dx < 0.0f ? -1.0f : 1.0f;
		sy2 = -1;
		sy3 = 1;
	}
	else
	{
		sx2 = -1;
		sx3 = 1;
		sy2 = sy3 = dy < 0.0f ? -1.0f : 1.0f;
	}

	float x2 = x1 + sx2;
	float x3 = x1 + sx3;
	float y2 = y1 + sy2;
	float y3 = y1 + sy3;

	// get heigts for triangle vertices

	float h1 = GetHeight((int)x1, (int)y1);
	float h2 = GetHeight((int)x2, (int)y2);
	float h3 = GetHeight((int)x3, (int)y3);

	// calculate barycentric coordinates k1, k2, k3
	// note: scale of each k is twice the normal

	float k2 = dx * sx2 + dy * sy2;
	float k3 = dx * sx3 + dy * sy3;
	float k1 = 2.0f - k2 - k3;

	return 0.5f * (k1 * h1 + k2 * h2 + k3 * h3);
}



static bool almost_zero(float value)
{
	static const float epsilon = 10 * std::numeric_limits<float>::epsilon();
	return fabsf(value) < epsilon;
}


const float* HeightMap::Intersect(ray r)
{
	glm::vec3 offset = glm::vec3(_bounds.min, 0);
	glm::vec3 scale = glm::vec3(glm::vec2(_cacheStride, _cacheStride) / _bounds.size(), 1);

	ray r2 = ray(scale * (r.origin - offset), glm::normalize(scale * r.direction));
	const float* d = InternalIntersect(r2);
	if (d == nullptr)
		return nullptr;

	static float result;
	result = glm::length((r2.point(*d) - r2.origin) / scale);
	return &result;
}


const float* HeightMap::InternalIntersect(ray r)
{
	static float result;

	bounds1f height = bounds1f(-2.5f, 250);
	bounds2f bounds(0, 0, _cacheMaxIndex, _cacheMaxIndex);
	bounds2f quad(-0.01f, -0.01f, 1.01f, 1.01f);

	const float* d = ::intersect(r, bounds3f(bounds, height));
	if (d == nullptr)
		return nullptr;

	glm::vec3 p = r.point(*d);

	bounds2f bounds_2(0, 0, _cacheMaxIndex - 1, _cacheMaxIndex - 1);

	int x = (int)bounds_2.x().clamp(p.x);
	int y = (int)bounds_2.y().clamp(p.y);
	int flipX = r.direction.x < 0 ? 0 : 1;
	int flipY = r.direction.y < 0 ? 0 : 1;
	int dx = r.direction.x < 0 ? -1 : 1;
	int dy = r.direction.y < 0 ? -1 : 1;

	height = height.grow(0.1f); // workaround for intersect precision problem

	while (height.contains(p.z) && bounds_2.contains(x, y))
	{
		glm::vec3 p00 = glm::vec3(x, y, GetHeight(x, y));
		glm::vec3 p10 = glm::vec3(x + 1, y, GetHeight(x + 1, y));
		glm::vec3 p01 = glm::vec3(x, y + 1, GetHeight(x, y + 1));
		glm::vec3 p11 = glm::vec3(x + 1, y + 1, GetHeight(x + 1, y + 1));

		if ((x & 1) == (y & 1))
		{
			d = ::intersect(r, plane(p00, p10, p11));
			if (d != nullptr)
			{
				glm::vec2 rel = (r.point(*d) - p00).xy();
				if (quad.contains(rel) && rel.x >= rel.y)
				{
					result = *d;
					return &result;
				}
			}

			d = ::intersect(r, plane(p00, p11, p01));
			if (d != nullptr)
			{
				glm::vec2 rel = (r.point(*d) - p00).xy();
				if (quad.contains(rel) && rel.x <= rel.y)
				{
					result = *d;
					return &result;
				}
			}
		}
		else
		{
			d = ::intersect(r, plane(p11, p01, p10));
			if (d != nullptr)
			{
				glm::vec2 rel = (r.point(*d) - p00).xy();
				if (quad.contains(rel) && rel.x >= 1 - rel.y)
				{
					result = *d;
					return &result;
				}
			}

			d = ::intersect(r, plane(p00, p10, p01));
			if (d != nullptr)
			{
				glm::vec2 rel = (r.point(*d) - p00).xy();
				if (quad.contains(rel) && rel.x <= 1 - rel.y)
				{
					result = *d;
					return &result;
				}
			}
		}

		float xDist = almost_zero(r.direction.x) ? std::numeric_limits<float>::max() : (x - p.x + flipX) / r.direction.x;
		float yDist = almost_zero(r.direction.y) ? std::numeric_limits<float>::max() : (y - p.y + flipY) / r.direction.y;

		if (xDist < yDist)
		{
			x += dx;
			p += r.direction * xDist;
		}
		else
		{
			y += dy;
			p += r.direction * yDist;
		}
	}

	return nullptr;
}


void HeightMap::UpdateHeights(std::function<float(int, int)> calculateHeight)
{
	int n = _cacheMaxIndex;

	for (int x = 0; x <= n; x += 2)
		for (int y = 0; y <= n; y += 2)
		{
			int i = x + y * _cacheStride;
			_cacheHeights[i] = calculateHeight(x, y);
		}

	for (int x = 1; x < n; x += 2)
		for (int y = 1; y < n; y += 2)
		{
			int i = x + y * _cacheStride;
			_cacheHeights[i] = calculateHeight(x, y);
		}

	for (int y = 0; y <= n; y += 2)
		for (int x = 1; x < n; x += 2)
		{
			int i = x + y * _cacheStride;
			_cacheHeights[i] = 0.5f * (_cacheHeights[i - 1] + _cacheHeights[i + 1]);
		}

	for (int y = 1; y < n; y += 2)
		for (int x = 0; x <= n; x += 2)
		{
			int i = x + y * _cacheStride;
			_cacheHeights[i] = 0.5f * (_cacheHeights[i - _cacheStride] + _cacheHeights[i + _cacheStride]);
		}
}


void HeightMap::UpdateNormals()
{
	int n = _cacheMaxIndex;
	glm::vec2 delta = 2.0f * _bounds.size() / (float)_cacheMaxIndex;
	for (int y = 0; y <= n; ++y)
	{
		for (int x = 0; x <= n; ++x)
		{
			int index = x + y * _cacheStride;
			int index_xn = x != 0 ? index - 1 : index;
			int index_xp = x != n ? index + 1 : index;
			int index_yn = y != 0 ? index - _cacheStride : index;
			int index_yp = y != n ? index + _cacheStride : index;

			float delta_hx = _cacheHeights[index_xp] - _cacheHeights[index_xn];
			float delta_hy = _cacheHeights[index_yp] - _cacheHeights[index_yn];

			glm::vec3 v1 = glm::vec3(delta.x, 0, delta_hx);
			glm::vec3 v2 = glm::vec3(0, delta.y, delta_hy);

			_cacheNormals[index] = glm::normalize(glm::cross(v1, v2));
		}
	}
}
