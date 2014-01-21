// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "../../BattleModel/GroundMap.h"
#include "SmoothTerrainWater.h"


SmoothTerrainWater::SmoothTerrainWater(GroundMap* groundMap) :
_groundMap(groundMap)
{
	_water_inside_renderer = new renderer1<glm::vec2>(
		"position",
		VERTEX_SHADER
		({
			uniform mat4 transform;
			uniform vec4 map_bounds;
			attribute vec2 position;

			void main()
			{
				vec4 p = transform * vec4(position, 0, 1);

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			void main()
			{
				gl_FragColor = vec4(0.44 * 0.5, 0.72 * 0.5, 0.91 * 0.5, 0.5);
				}
			})
	);
	_water_inside_renderer->_blend_sfactor = GL_ONE;
	_water_inside_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



	_water_border_renderer = new renderer1<glm::vec2>(
		"position",
		VERTEX_SHADER
		({
			uniform mat4 transform;
			uniform vec4 map_bounds;
			attribute vec2 position;
			varying vec2 _groundpos;

			void main()
			{
				vec4 p = transform * vec4(position, 0, 1);

				_groundpos = (position - map_bounds.xy) / map_bounds.zw;

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			varying vec2 _groundpos;

			void main()
			{
				if (distance(_groundpos, vec2(0.5, 0.5)) > 0.5)
					discard;

				gl_FragColor = vec4(0.44 * 0.5, 0.72 * 0.5, 0.91 * 0.5, 0.5);
			}
		})
	);
	_water_border_renderer->_blend_sfactor = GL_ONE;
	_water_border_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;

	Update();
}


SmoothTerrainWater::~SmoothTerrainWater()
{
	delete _water_inside_renderer;
	delete _water_border_renderer;
}


static int inside_circle(bounds2f bounds, glm::vec2 p)
{
	return glm::distance(p, bounds.center()) <= bounds.width() / 2 ? 1 : 0;
}


static int inside_circle(bounds2f bounds, plain_vertex v1, plain_vertex v2, plain_vertex v3)
{
	return inside_circle(bounds, v1._1)
		+ inside_circle(bounds, v2._1)
		+ inside_circle(bounds, v3._1);

}


static vertexbuffer<plain_vertex>* choose_shape(int count, vertexbuffer<plain_vertex>* inside, vertexbuffer<plain_vertex>* border)
{
	switch (count)
	{
		case 1:
		case 2:
			return border;

		case 3:
			return inside;

		default:
			return nullptr;
	}
}

void SmoothTerrainWater::Update()
{
	bounds2f bounds = _groundMap->GetBounds();

	_shape_water_inside._mode = GL_TRIANGLES;
	_shape_water_border._mode = GL_TRIANGLES;

	_shape_water_inside._vertices.clear();
	_shape_water_border._vertices.clear();

	int n = 64;
	glm::vec2 s = bounds.size() / (float)n;
	for (int x = 0; x < n; ++x)
		for (int y = 0; y < n; ++y)
		{
			glm::vec2 p = bounds.min + s * glm::vec2(x, y);
			if (_groundMap->ContainsWater(bounds2f(p, p + s)))
			{
				plain_vertex v11 = plain_vertex(p);
				plain_vertex v12 = plain_vertex(p + glm::vec2(0, s.y));
				plain_vertex v21 = plain_vertex(p + glm::vec2(s.x, 0));
				plain_vertex v22 = plain_vertex(p + s);

				vertexbuffer<plain_vertex>* s = choose_shape(inside_circle(bounds, v11, v22, v12), &_shape_water_inside, &_shape_water_border);
				if (s != nullptr)
				{
					s->_vertices.push_back(v11);
					s->_vertices.push_back(v22);
					s->_vertices.push_back(v12);
				}

				s = choose_shape(inside_circle(bounds, v22, v11, v21), &_shape_water_inside, &_shape_water_border);
				if (s != nullptr)
				{
					s->_vertices.push_back(v22);
					s->_vertices.push_back(v11);
					s->_vertices.push_back(v21);
				}
			}
		}

	_shape_water_inside.update(GL_STATIC_DRAW);
	_shape_water_border.update(GL_STATIC_DRAW);
}


void SmoothTerrainWater::Render(const glm::mat4x4& transform)
{
	bounds2f bounds = _groundMap->GetBounds();

	ground_texture_uniforms uniforms;
	uniforms._transform = transform;
	uniforms._map_bounds = glm::vec4(bounds.min, bounds.size());
	uniforms._texture = nullptr;

	_water_inside_renderer->get_uniform<glm::mat4>("transform").set_value(uniforms._transform);
	_water_inside_renderer->get_uniform<glm::vec4>("map_bounds").set_value(uniforms._map_bounds);
	_water_inside_renderer->get_uniform<const texture*>("texture").set_value(uniforms._texture);
	_water_inside_renderer->render(_shape_water_inside);

	_water_border_renderer->get_uniform<glm::mat4>("transform").set_value(uniforms._transform);
	_water_border_renderer->get_uniform<glm::vec4>("map_bounds").set_value(uniforms._map_bounds);
	_water_border_renderer->get_uniform<const texture*>("texture").set_value(uniforms._texture);
	_water_border_renderer->render(_shape_water_border);
}
