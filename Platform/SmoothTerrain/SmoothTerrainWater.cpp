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


static int inside_circle(bounds2f bounds, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
	return inside_circle(bounds, p1)
		+ inside_circle(bounds, p2)
		+ inside_circle(bounds, p3);

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
				glm::vec2 v11 = p;
				glm::vec2 v12 = p + glm::vec2(0, s.y);
				glm::vec2 v21 = p + glm::vec2(s.x, 0);
				glm::vec2 v22 = p + s;

				VertexShape_2f* s = choose_shape(inside_circle(bounds, v11, v22, v12), &_waterInsideVertices, &_waterBorderVertices);
				if (s != nullptr)
				{
					s->AddVertex(Vertex_2f(v11));
					s->AddVertex(Vertex_2f(v22));
					s->AddVertex(Vertex_2f(v12));
				}

				s = choose_shape(inside_circle(bounds, v22, v11, v21), &_waterInsideVertices, &_waterBorderVertices);
				if (s != nullptr)
				{
					s->AddVertex(Vertex_2f(v22));
					s->AddVertex(Vertex_2f(v11));
					s->AddVertex(Vertex_2f(v21));
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
