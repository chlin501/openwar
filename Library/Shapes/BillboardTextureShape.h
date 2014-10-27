// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TextureBillboardRenderer_H
#define TextureBillboardRenderer_H

#include <map>
#include <vector>

#include "Algebra/affine2.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/texture.h"
#include "Graphics/Image.h"
#include "VertexShape.h"


class BillboardTexture
{
	struct item
	{
		int shape;
		float facing; // degrees
		affine2 texcoords;
		item(int s, float f, affine2 t) : shape(s), facing(f), texcoords(t) { }
	};

	texture* _texture;
	std::map<int, std::vector<item>> _items;
	int _shapeCount;

public:
	BillboardTexture(GraphicsContext* gc);
	~BillboardTexture();

	texture* GetTexture() const { return _texture; }

	int AddSheet(const Image& img);
	int AddShape(int sheet);

	void SetTexCoords(int shape, float facing, const affine2& texcoords);

	affine2 GetTexCoords(int shape, float facing);
};


struct Billboard
{
	glm::vec3 position;
	float facing;
	float height;
	int shape;

	Billboard() {}
	Billboard(glm::vec3 p, float f, float h, int s) : position(p), facing(f), height(h), shape(s) {}
};


struct BillboardModel
{
	BillboardTexture* texture;
	std::vector<Billboard> staticBillboards;
	std::vector<Billboard> dynamicBillboards;

	int _billboardTreeShapes[16];
	int _billboardShapeCasualtyAsh[8];
	int _billboardShapeCasualtySam[8];
	int _billboardShapeCasualtyCav[16];
	int _billboardShapeFighterSamBlue;
	int _billboardShapeFighterSamRed;
	int _billboardShapeFighterAshBlue;
	int _billboardShapeFighterAshRed;
	int _billboardShapeFighterCavBlue;
	int _billboardShapeFighterCavRed;
	int _billboardShapeSmoke[8];
};


class BillboardTextureShader : public ShaderProgram4<glm::vec3, float, glm::vec2, glm::vec2>
{
	friend class GraphicsContext;
	BillboardTextureShader(GraphicsContext* gc);
};


class BillboardTextureShape
{
public:
	VertexShape_3f_1f_2f_2f _vertices;

	BillboardTextureShape();
	~BillboardTextureShape();

	void Reset();
	void AddBillboard(glm::vec3 position, float height, affine2 texcoords);

	void Draw(GraphicsContext* gc, texture* tex, const glm::mat4x4& transform, const glm::vec3& cameraUp, float cameraFacingDegrees, float viewportHeight, bounds1f sizeLimit = bounds1f(0, 1024));

	void Render(GraphicsContext* gc, BillboardModel* billboardModel, const glm::mat4x4& transform, const glm::vec3& cameraUp, float viewportHeight, float cameraFacingDegrees, bool flip);
};


#endif
