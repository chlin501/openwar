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
	FrameBufferX* _framebuffer;
	RenderBufferX* _colorbuffer;
	texture* _depth;
	texture* _colormap;
	texture* _splatmap;

	SmoothTerrainShaders* _renderers;
	VertexBufferX<Vertex_2f> _vboShadow;
	VertexBufferX<terrain_vertex> _vboInside;
	VertexBufferX<terrain_vertex> _vboBorder;
	VertexBufferX<skirt_vertex> _vboSkirt;
	VertexBufferX<Vertex_3f> _vboLines;

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

	VertexBufferX<terrain_vertex>* SelectTerrainVbo(int inside);

	void BuildTriangles();
	void PushTriangle(const terrain_vertex& v0, const terrain_vertex& v1, const terrain_vertex& v2);
};


#endif
