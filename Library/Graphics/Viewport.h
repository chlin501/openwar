// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Viewport_H
#define Viewport_H

#include "Algebra/bounds.h"

class FrameBuffer;
class GraphicsContext;


class Viewport
{
	GraphicsContext* _gc{};
	bounds2i _bounds{};
	FrameBuffer* _frameBuffer{};

public:
	Viewport(GraphicsContext* gc);
	virtual ~Viewport();

	GraphicsContext* GetGraphicsContext() const;

	FrameBuffer* GetFrameBuffer() const;
	void SetFrameBuffer(FrameBuffer* frameBuffer);

	bounds2i GetViewportBounds() const;
	void SetViewportBounds(const bounds2i& value);

	virtual glm::mat4 GetTransform() const = 0;

	glm::vec2 GlobalToNormalized(glm::vec2 value) const;
	glm::vec2 NormalizedToGlobal(glm::vec2 value) const;
};


class BasicViewport : public Viewport
{
public:
	BasicViewport(GraphicsContext* gc);

	glm::mat4 GetTransform() const override;
};


#endif
