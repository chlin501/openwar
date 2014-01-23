// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SURFACE_H
#define SURFACE_H

#include <vector>

#include "../Algebra/bounds.h"
#include "graphicscontext.h"

class Gesture;
class Touch;


class Surface
{
	graphicscontext* _gc;
	float _pixelDensity;
	glm::vec2 _position;
	glm::vec2 _size;

public:
	std::vector<Gesture*> _gestures;

	Surface(glm::vec2 size, float pixelDensity);
	virtual ~Surface();

	graphicscontext* GetGraphicsContext() const { return _gc; }

	virtual glm::vec2 GetPosition() const { return _position; }
	virtual void SetPosition(glm::vec2 value) { _position = value; }

	glm::vec2 GetSize() const { return _size; }
	void SetSize(glm::vec2 value) { _size = value; }

	void UseViewport();

	bounds2f GetSpriteViewport() const;

	virtual void ScreenSizeChanged();

	virtual bool NeedsRender() const = 0;
	virtual void RenderSurface() = 0;
	virtual void Update(double secondsSinceLastUpdate) = 0;

	virtual bool ShowContextualMenu(glm::vec2 position);

	virtual void MouseEnter(glm::vec2 position);
	virtual void MouseHover(glm::vec2 position);
	virtual void MouseLeave(glm::vec2 position);
};


#endif
