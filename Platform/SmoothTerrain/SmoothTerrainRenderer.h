// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SmoothTerrainRenderer_H
#define SmoothTerrainRenderer_H

#include "Algebra/bounds.h"
#include "BattleModel/GroundMap.h"
#include "BattleModel/HeightMap.h"
#include "BattleModel/SmoothGroundMap.h"
#include "Shapes/VertexShape.h"
#include "SmoothTerrainShaders.h"

class FrameBuffer;
class Image;
class RenderBuffer;


class SmoothTerrainRenderer
{
	GraphicsContext* _gc;
	SmoothGroundMap* _smoothGroundMap;
	int _framebuffer_width{};
	int _framebuffer_height{};

	FrameBuffer* _sobelFrameBuffer{};
	RenderBuffer* _sobelColorBuffer{};
	Texture* _sobelDepthBuffer{};

	Texture* _hatchingsMasterColorBuffer{};
	glm::ivec2 _hatchingsIntermediateBufferSize;
	FrameBuffer* _hatchingsIntermediateFrameBuffer{};
	Texture* _hatchingsIntermediateColorBuffer{};
	Texture* _hatchingsIntermediateDepthBuffer{};
	Texture* _hatchingsPatternR{};
	Texture* _hatchingsPatternG{};
	Texture* _hatchingsPatternB{};

	Texture* _colormap{};
	Texture* _splatmap{};

	VertexShape_2f _shadowVertices;
	VertexShape_3f_3f _insideVertices;
	VertexShape_3f_3f _borderVertices;
	VertexShape_3f_1f _skirtVertices;
	VertexShape_3f _lineVertices;

	VertexShape_2f_2f _hatchingsVertices;

	bool _showLines{};
	bool _editMode{};

public:
	SmoothTerrainRenderer(GraphicsContext* gc, SmoothGroundMap* smoothGroundMap);
	virtual ~SmoothTerrainRenderer();

	SmoothGroundMap* GetSmoothGroundMap() const { return _smoothGroundMap; }

	void EnableSobelBuffers();
	void UpdateSobelBufferSize();

	void EnableHatchingsBuffers();

	void Render(const glm::mat4& transform, const glm::vec3& lightNormal);
	void RenderHatchings(const glm::mat4& transform);

	void UpdateChanges(bounds2f bounds);
	void UpdateSplatmap();

	void InitializeShadow();
	void InitializeSkirt();
	void InitializeLines();

	VertexShape_3f_3f* SelectTerrainVertexBuffer(int inside);

	void BuildTriangles();
	void PushTriangle(const Vertex_3f_3f& v0, const Vertex_3f_3f& v1, const Vertex_3f_3f& v2);

private:
	static Texture* CreateColorMap(GraphicsContext* gc);
};


#endif
