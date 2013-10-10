// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainRenderer_H
#define SmoothTerrainRenderer_H

#include "../../Library/Algebra/bounds.h"
#include "GroundMap.h"
#include "SmoothTerrainShaders.h"
#include "SmoothGroundMap.h"
#include "HeightMap.h"

class image;



class SmoothTerrainRenderer
{
	SmoothGroundMap* _smoothGroundMap;
	int _framebuffer_width;
	int _framebuffer_height;
	framebuffer* _framebuffer;
	renderbuffer* _colorbuffer;
	texture* _depth;
	texture* _colormap;
	texture* _splatmap;

	SmoothTerrainShaders* _renderers;
	vertexbuffer<plain_vertex> _vboShadow;
	vertexbuffer<terrain_vertex> _vboInside;
	vertexbuffer<terrain_vertex> _vboBorder;
	vertexbuffer<skirt_vertex> _vboSkirt;
	vertexbuffer<plain_vertex3> _vboLines;

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

	vertexbuffer<terrain_vertex>* SelectTerrainVbo(int inside);

	void BuildTriangles();
	void PushTriangle(const terrain_vertex& v0, const terrain_vertex& v1, const terrain_vertex& v2);
};


#endif
