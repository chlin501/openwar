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

class image;



class SmoothTerrainRenderer
{
	SmoothGroundMap* _smoothGroundMap;
	int _framebuffer_width;
	int _framebuffer_height;
	FrameBuffer* _framebuffer;
	RenderBuffer* _colorbuffer;
	texture* _depth;
	texture* _colormap;
	texture* _splatmap;

	SmoothTerrainShaders* _renderers;
	VertexBuffer_2f _shadowVertices;
	VertexBuffer_3f_3f _insideVertices;
	VertexBuffer_3f_3f _borderVertices;
	VertexBuffer_3f_1f _skirtVertices;
	VertexBuffer_3f _lineVertices;

	bool _showLines;
	bool _editMode;

public:
	SmoothTerrainRenderer(SmoothGroundMap* smoothGroundMap);
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

	VertexBuffer_3f_3f* SelectTerrainVertexBuffer(int inside);

	void BuildTriangles();
	void PushTriangle(const Vertex_3f_3f& v0, const Vertex_3f_3f& v1, const Vertex_3f_3f& v2);
};


#endif
