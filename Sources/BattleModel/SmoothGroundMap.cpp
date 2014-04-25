// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SmoothGroundMap.h"
#include "HeightMap.h"


SmoothGroundMap::SmoothGroundMap(HeightMap* heightMap, const char* name, bounds2f bounds, image* img) :
_heightMap(heightMap),
_name(name),
_bounds(bounds),
_image(img)
{
	UpdateHeightMap();
}


glm::ivec2 SmoothGroundMap::ToGroundmapCoordinate(glm::vec2 position) const
{
	glm::vec2 p = (position - _bounds.min) / _bounds.size();
	glm::ivec2 s = _image->size();
	return glm::ivec2((int)(p.x * s.x), (int)(p.y * s.y));
}


float SmoothGroundMap::CalculateHeight(int x, int y) const
{
	if (x < 1) x = 1; else if (x > _image->size().x - 1) x = _image->size().x - 1;
	if (y < 1) y = 1; else if (y > _image->size().y - 1) y = _image->size().y - 1;

	glm::vec4 color = _image->get_pixel(x, y);
	glm::vec4 color_xn = _image->get_pixel(x - 1, y);
	glm::vec4 color_xp = _image->get_pixel(x + 1, y);
	glm::vec4 color_yn = _image->get_pixel(x, y - 1);
	glm::vec4 color_yp = _image->get_pixel(x, y + 1);

	float alpha = 0.5f * color.a + 0.125f * (color_xn.a + color_xp.a + color_yn.a + color_yp.a);

	float height = 0.5f + 124.5f * (1.0f - alpha);

	float water = color.b;
	height = glm::mix(height, -2.5f, water);

	float fords = color.r;
	height = glm::mix(height, -0.5f, water * fords);

	return height;
}


bool SmoothGroundMap::IsForest(glm::vec2 position) const
{
	glm::ivec2 coord = ToGroundmapCoordinate(position);
	return GetForestValue(coord.x, coord.y) >= 0.5;
}


bool SmoothGroundMap::IsImpassable(glm::vec2 position) const
{
	glm::ivec2 coord = ToGroundmapCoordinate(position);
	return GetImpassableValue(coord.x, coord.y) >= 0.5;
}


bool SmoothGroundMap::IsWater(glm::vec2 position) const
{
	glm::ivec2 mapsize = _image->size();
	glm::vec2 p = (position - _bounds.min) / _bounds.size();
	int x = (int)(mapsize.x * glm::floor(p.x));
	int y = (int)(mapsize.y * glm::floor(p.y));
	glm::vec4 c = _image->get_pixel(x, y);
	return c.b >= 0.5;
}


bool SmoothGroundMap::ContainsWater(bounds2f bounds) const
{
	glm::ivec2 mapsize = _image->size();
	glm::vec2 min = glm::vec2(mapsize.x - 1, mapsize.y - 1) * (bounds.min - _bounds.min) / _bounds.size();
	glm::vec2 max = glm::vec2(mapsize.x - 1, mapsize.y - 1) * (bounds.max - _bounds.min) / _bounds.size();
	int xmin = (int)floorf(min.x);
	int ymin = (int)floorf(min.y);
	int xmax = (int)ceilf(max.x);
	int ymax = (int)ceilf(max.y);

	for (int x = xmin; x <= xmax; ++x)
		for (int y = ymin; y <= ymax; ++y)
		{
			glm::vec4 c = _image->get_pixel(x, y);
			if (c.b >= 0.5)
				return true;
		}

	return false;
}


float SmoothGroundMap::GetForestValue(int x, int y) const
{
	glm::vec4 c = _image->get_pixel(x, y);
	return c.g;
}


float SmoothGroundMap::GetImpassableValue(int x, int y) const
{
	if (0 <= x && x < 255 && 0 <= y && y < 255)
	{
		glm::vec4 c = _image->get_pixel(x, y);
		if (c.b >= 0.5f && c.r < 0.5f)
			return 1.0f;

		glm::vec3 n = _heightMap->GetNormal(x, y);

		return bounds1f(0, 1).clamp(0.5f + 8.0f * (0.83f - n.z));
	}
	return 0;
}


void SmoothGroundMap::Extract(glm::vec2 position, image* brush)
{
	glm::ivec2 size = brush->size();
	glm::ivec2 origin = ToGroundmapCoordinate(position) - size / 2;

	for (int x = 0; x < size.x; ++x)
		for (int y = 0; y < size.y; ++y)
			brush->set_pixel(x, y, _image->get_pixel(origin.x + x, origin.y + y));
}


bounds2f SmoothGroundMap::Paint(TerrainFeature feature, glm::vec2 position, image* brush, float pressure)
{
	glm::vec2 scale = _bounds.size() / glm::vec2(_image->size());
	glm::ivec2 size = brush->size();
	glm::ivec2 center = ToGroundmapCoordinate(position);
	glm::ivec2 origin = center - size / 2;
	float radius = size.x / 2.0f;

	for (int x = 0; x < size.x; ++x)
		for (int y = 0; y < size.y; ++y)
		{
			glm::ivec2 p = origin + glm::ivec2(x, y);
			float d = glm::distance(position, scale * glm::vec2(p)) / radius;
			float k = 1.0f - d * d;
			if (k > 0)
			{
				glm::vec4 b = brush->get_pixel(x, y);
				glm::vec4 c = _image->get_pixel(p.x, p.y);
				switch (feature)
				{
					case TerrainFeature::Hills:
						c.a = glm::mix(c.a, b.a, k * pressure);
						break;
					case TerrainFeature::Trees:
						c.g = glm::mix(c.g, b.g, k * pressure);
						break;
					case TerrainFeature::Water:
						c.b = glm::mix(c.b, b.b, k * pressure);
						break;
					case TerrainFeature::Fords:
						c.r = glm::mix(c.r, b.r, k * pressure);
						break;
				}
				_image->set_pixel(p.x, p.y, c);
			}
		}

	UpdateHeightMap();

	return bounds2_from_center(position, radius + 1);
}


bounds2f SmoothGroundMap::Paint(TerrainFeature feature, glm::vec2 position, float radius, float pressure)
{
	glm::vec2 scale = _bounds.size() / glm::vec2(_image->size());
	float abs_pressure = glm::abs(pressure);

	glm::ivec2 center = ToGroundmapCoordinate(position);

	float value = pressure > 0 ? 1 : 0;
	float delta = pressure > 0 ? -0.015f : 0.015f;

	for (int x = -10; x <= 10; ++x)
		for (int y = -10; y <= 10; ++y)
		{
			glm::ivec2 p = center + glm::ivec2(x, y);
			float d = glm::distance(position, scale * glm::vec2(p)) / radius;
			float k = 1.0f - d * d;
			if (k > 0)
			{
				glm::vec4 c = _image->get_pixel(p.x, p.y);
				switch (feature)
				{
					case TerrainFeature::Hills:
						c.a = glm::mix(c.a, c.a + delta, k * abs_pressure);
						break;
					case TerrainFeature::Trees:
						c.g = glm::mix(c.g, value, k * abs_pressure);
						break;
					case TerrainFeature::Water:
						c.b = glm::mix(c.b, value, k * abs_pressure);
						break;
					case TerrainFeature::Fords:
						c.r = glm::mix(c.r, value, k * abs_pressure);
						break;
				}
				_image->set_pixel(p.x, p.y, c);
			}
		}

	UpdateHeightMap();

	return bounds2_from_center(position, radius + 1);
}


void SmoothGroundMap::UpdateHeightMap()
{
	_heightMap->Update([this](int x, int y) { return CalculateHeight(x, y); });
}
