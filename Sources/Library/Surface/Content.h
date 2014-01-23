// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef CONTENT_H
#define CONTENT_H

#include "Surface.h"


glm::mat4 ViewportTransform(bounds2f viewport, glm::vec2 translate = glm::vec2(), float rotate = 0);


class Content
{
	Surface* _surface;
	bounds2f _frame;
	float _aspect;
	bool _flip;

public:
	Content(Surface* surface);
	virtual ~Content();

	Surface* GetSurface() const { return _surface; }

	bounds2f GetFrame() const;
	virtual void SetFrame(bounds2f value);
	float GetFrameAspect() const { return _aspect; }

	void UseViewport();

	bool GetFlip() const { return _flip; }
	void SetFlip(bool value) { _flip = value; }

	glm::vec2 SurfaceToContent(glm::vec2 value) const;
	glm::vec2 ContentToSurface(glm::vec2 value) const;

	virtual void ScreenSizeChanged();

	virtual void RenderContent() = 0;
	virtual void Update(double secondsSinceLastUpdate) = 0;

private:
	void UpdateAspect();
};



#endif
