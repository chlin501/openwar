// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Viewport_H
#define Viewport_H

#include "Algebra/bounds.h"

class GraphicsContext;


class Viewport
{
	GraphicsContext* _gc;
	bounds2i _bounds;

public:
	Viewport(GraphicsContext* gc);
	virtual ~Viewport();

	virtual bounds2i GetBounds() const;
	virtual void SetBounds(const bounds2i& value);
	virtual void UseViewport() const;

	virtual glm::mat4 GetTransform() const = 0;

	glm::vec2 GlobalToNormalized(glm::vec2 value) const;
	glm::vec2 NormalizedToGlobal(glm::vec2 value) const;
};


#endif
