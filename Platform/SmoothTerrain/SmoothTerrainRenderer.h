// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainRenderer_H
#define SmoothTerrainRenderer_H

#include "BattleModel/GroundMap.h"
#include "BattleModel/HeightMap.h"
#include "BattleModel/SmoothGroundMap.h"
#include "Algebra/bounds.h"
#include "SmoothTerrainShaders.h"

class Image;



class SmoothTerrainRenderer
{
	GraphicsContext* _gc;
	SmoothGroundMap* _smoothGroundMap;
	int _framebuffer_width;
	int _framebuffer_height;
	FrameBuffer* _framebuffer;
	RenderBuffer* _colorbuffer;
	Texture* _depth;
	Texture* _colormap;
	Texture* _splatmap;

	SmoothTerrainShaders* _renderers;
	VertexShape_2f _shadowVertices;
	VertexShape_3f_3f _insideVertices;
	VertexShape_3f_3f _borderVertices;
	VertexShape_3f_1f _skirtVertices;
	VertexShape_3f _lineVertices;

	bool _showLines;
	bool _editMode;

public:
	SmoothTerrainRenderer(GraphicsContext* gc, SmoothGroundMap* smoothGroundMap);
	virtual ~SmoothTerrainRenderer();

	SmoothGroundMap* GetSmoothGroundMap() const { return _smoothGroundMap; }

	void Render(const glm::mat4x4& transform, const glm::vec3& lightNormal);

	void EnableRenderEdges();

	void UpdateChanges(bounds2f bounds);
	void UpdateDepthTextureSize();
	void UpdateSplatmap();

	void InitializeShadow();
	void InitializeSkirt();
	void InitializeLines();

	VertexShape_3f_3f* SelectTerrainVertexBuffer(int inside);

	void BuildTriangles();
	void PushTriangle(const Vertex_3f_3f& v0, const Vertex_3f_3f& v1, const Vertex_3f_3f& v2);
};


#endif
