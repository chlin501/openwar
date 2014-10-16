// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleModel/HeightMap.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "TiledTerrainRenderer.h"


TiledTerrainRenderer::TiledTerrainRenderer(TiledGroundMap* tiledGroundMap) :
_tiledGroundMap(tiledGroundMap)
{
	glm::ivec2 size = _tiledGroundMap->GetSize();
	for (int x = 0; x < size.x; ++x)
		for (int y = 0; y < size.y; ++y)
		{
			TiledGroundMap::Tile* tile = _tiledGroundMap->GetTile(x, y);

			if (_textures.find(tile->texture) == _textures.end())
			{
				::texture* t = new ::texture(image(resource(tile->texture.c_str())));

				glBindTexture(GL_TEXTURE_2D, t->id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				_textures[tile->texture] = t;
			}
		}
}


TiledTerrainRenderer::~TiledTerrainRenderer()
{
}




void TiledTerrainRenderer::Render(const glm::mat4x4& transform, const glm::vec3& lightNormal)
{
	//HeightMap* heightMap = _tiledGroundMap->GetHeightMap();
	bounds2f bounds = _tiledGroundMap->GetBounds();
	glm::ivec2 size = _tiledGroundMap->GetSize();

	VertexBuffer_3f_2f shape;
	shape._mode = GL_TRIANGLES;

	glm::vec2 delta = bounds.size() / glm::vec2(size);

	for (int x = 0; x < size.x; ++x)
		for (int y = 0; y < size.y; ++y)
		{
			TiledGroundMap::Tile* tile = _tiledGroundMap->GetTile(x, y);

			glm::vec2 p0 = bounds.min + delta * glm::vec2(x, y);
			glm::vec2 p1 = p0 + delta;

			float h00 = _tiledGroundMap->GetHeight(x, y); //heightMap->InterpolateHeight(glm::vec2(p0.x, p0.y));
			float h01 = _tiledGroundMap->GetHeight(x, y + 1); //heightMap->InterpolateHeight(glm::vec2(p0.x, p1.y));
			float h10 = _tiledGroundMap->GetHeight(x + 1, y); //heightMap->InterpolateHeight(glm::vec2(p1.x, p0.y));
			float h11 = _tiledGroundMap->GetHeight(x + 1, y + 1); //heightMap->InterpolateHeight(glm::vec2(p1.x, p1.y));

			glm::vec2 t00 = glm::vec2(0, 0);
			glm::vec2 t01 = glm::vec2(0, 1);
			glm::vec2 t10 = glm::vec2(1, 0);
			glm::vec2 t11 = glm::vec2(1, 1);

			for (int i = 0; i < 3; ++i)
			{
				glm::vec2 tmp = t00;
				t00 = t01;
				t01 = t11;
				t11 = t10;
				t10 = tmp;
			}

			if (tile->mirror)
			{
				t00.x = 1 - t00.x;
				t01.x = 1 - t01.x;
				t10.x = 1 - t10.x;
				t11.x = 1 - t11.x;
			}

			for (int i = 0; i < tile->rotate; ++i)
			{
				glm::vec2 tmp = t00;
				t00 = t01;
				t01 = t11;
				t11 = t10;
				t10 = tmp;
			}

			shape._vertices.clear();
			shape._vertices.push_back(Vertex_3f_2f(glm::vec3(p0.x, p0.y, h00), t01));
			shape._vertices.push_back(Vertex_3f_2f(glm::vec3(p1.x, p0.y, h10), t11));
			shape._vertices.push_back(Vertex_3f_2f(glm::vec3(p1.x, p1.y, h11), t10));
			shape._vertices.push_back(Vertex_3f_2f(glm::vec3(p1.x, p1.y, h11), t10));
			shape._vertices.push_back(Vertex_3f_2f(glm::vec3(p0.x, p1.y, h01), t00));
			shape._vertices.push_back(Vertex_3f_2f(glm::vec3(p0.x, p0.y, h00), t01));

			shape.update(GL_STATIC_DRAW);

			renderers::singleton->_texture_renderer3->get_uniform<glm::mat4>("transform").set_value(transform);
			renderers::singleton->_texture_renderer3->get_uniform<const texture*>("texture").set_value(_textures[tile->texture]);
			renderers::singleton->_texture_renderer3->render(shape);
		}
}
