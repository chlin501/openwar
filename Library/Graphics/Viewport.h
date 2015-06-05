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

	glm::vec2 ScreenToNormalized(glm::vec2 value) const;
	glm::vec2 NormalizedToScreen(glm::vec2 value) const;

	virtual glm::mat4 GetTransform() const = 0;
};


class Viewport2D : public Viewport
{
public:
	Viewport2D(GraphicsContext* gc);

	glm::vec2 ContentToNormalized(glm::vec2 value) const;
	glm::vec2 NormalizedToContent(glm::vec2 value) const;

	virtual glm::vec2 GetContentSize() const = 0;
	virtual bounds2f GetVisibleBounds() const = 0;

	virtual glm::vec2 ContentToScreen(glm::vec2 value) const = 0;
	virtual glm::vec2 ScreenToContent(glm::vec2 value) const = 0;
};


class StandardViewport : public Viewport2D
{
public:
	StandardViewport(GraphicsContext* gc);

public: // Viewport2D
	glm::vec2 GetContentSize() const override;
	bounds2f GetVisibleBounds() const override;

	glm::vec2 ContentToScreen(glm::vec2 value) const override;
	glm::vec2 ScreenToContent(glm::vec2 value) const override;

public: // Viewport
	glm::mat4 GetTransform() const override;
};


class ScrollerViewport : public Viewport2D
{
	glm::vec2 _contentOffset{};
	glm::vec2 _contentSize{};

public:
	ScrollerViewport(GraphicsContext* gc);

	void SetContentSize(glm::vec2 value);

	glm::vec2 GetContentOffset() const;
	void SetContentOffset(glm::vec2 value);

	glm::vec2 GetDefaultOffset() const;
	glm::vec2 GetClampedOffset(glm::vec2 value) const;
	void ClampContentOffset();

public: // Viewport2D
	glm::vec2 GetContentSize() const override;
	bounds2f GetVisibleBounds() const override;

	glm::vec2 ContentToScreen(glm::vec2 value) const override;
	glm::vec2 ScreenToContent(glm::vec2 value) const override;

public: // Viewport
	glm::mat4 GetTransform() const override;
};


#endif
