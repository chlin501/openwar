// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleModel/GroundMap.h"
#include "SmoothTerrainWater.h"
#include "Graphics/RenderCall.h"



WaterInsideShader::WaterInsideShader(GraphicsContext* gc) : ShaderProgram(
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
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


WaterBorderShader::WaterBorderShader(GraphicsContext* gc) : ShaderProgram(
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
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


SmoothTerrainWater::SmoothTerrainWater(GroundMap* groundMap) :
_groundMap(groundMap)
{
	Update();
}


SmoothTerrainWater::~SmoothTerrainWater()
{
}


static int inside_circle(bounds2f bounds, glm::vec2 p)
{
	return glm::distance(p, bounds.mid()) <= bounds.x().size() / 2 ? 1 : 0;
}


static int inside_circle(bounds2f bounds, Vertex_2f v1, Vertex_2f v2, Vertex_2f v3)
{
	return inside_circle(bounds, v1._1)
		+ inside_circle(bounds, v2._1)
		+ inside_circle(bounds, v3._1);

}


static VertexShape_2f* choose_shape(int count, VertexShape_2f* inside, VertexShape_2f* border)
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

	_waterInsideVertices._mode = GL_TRIANGLES;
	_waterBorderVertices._mode = GL_TRIANGLES;

	_waterInsideVertices.Clear();
	_waterBorderVertices.Clear();

	int n = 64;
	glm::vec2 s = bounds.size() / (float)n;
	for (int x = 0; x < n; ++x)
		for (int y = 0; y < n; ++y)
		{
			glm::vec2 p = bounds.min + s * glm::vec2(x, y);
			if (_groundMap->ContainsWater(bounds2f(p, p + s)))
			{
				Vertex_2f v11 = Vertex_2f(p);
				Vertex_2f v12 = Vertex_2f(p + glm::vec2(0, s.y));
				Vertex_2f v21 = Vertex_2f(p + glm::vec2(s.x, 0));
				Vertex_2f v22 = Vertex_2f(p + s);

				VertexShape_2f* s = choose_shape(inside_circle(bounds, v11, v22, v12), &_waterInsideVertices, &_waterBorderVertices);
				if (s != nullptr)
				{
					s->AddVertex(v11);
					s->AddVertex(v22);
					s->AddVertex(v12);
				}

				s = choose_shape(inside_circle(bounds, v22, v11, v21), &_waterInsideVertices, &_waterBorderVertices);
				if (s != nullptr)
				{
					s->AddVertex(v22);
					s->AddVertex(v11);
					s->AddVertex(v21);
				}
			}
		}
}


void SmoothTerrainWater::Render(GraphicsContext* gc, const glm::mat4x4& transform)
{
	bounds2f bounds = _groundMap->GetBounds();

	RenderCall<WaterInsideShader>(gc)
		.SetVertices(&_waterInsideVertices, "position")
		.SetUniform("transform", transform)
		.SetUniform("map_bounds", glm::vec4(bounds.min, bounds.size()))
		.SetTexture("texture", nullptr)
		.Render();

	RenderCall<WaterBorderShader>(gc)
		.SetVertices(&_waterBorderVertices, "position")
		.SetUniform("transform", transform)
		.SetUniform("map_bounds", glm::vec4(bounds.min, bounds.size()))
		.SetTexture("texture", nullptr)
		.Render();
}
