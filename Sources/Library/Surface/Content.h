// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef CONTENT_H
#define CONTENT_H

#include "bounds.h"

class Container;
class Surface;


glm::mat4 ViewportTransform(bounds2f viewport, glm::vec2 translate = glm::vec2(), float rotate = 0);



class Content
{
	friend void SetContentContainer(Content*, Container*);

	Surface* _surface;
	Container* _container;
	bounds2f _frame;
	glm::vec2 _anchor;
	bool _flip;

public:
	Content(Surface* surface);
	Content();
	virtual ~Content();

	Surface* GetSurface() const { return _surface; }

	virtual Container* GetContainer() const;
	virtual void SetContainer(Container* value);


	virtual bounds2f GetFrame() const;
	virtual void SetFrame(bounds2f value);
	virtual void OnFrameChanged();

	virtual glm::vec2 GetAnchor() const;
	virtual void SetAnchor(glm::vec2 value);

	virtual glm::vec2 GetPosition() const;
	virtual void SetPosition(glm::vec2 value);

	virtual glm::vec2 GetSize() const;
	virtual void SetSize(glm::vec2 value);


	virtual void UseViewport();

	bool GetFlip() const { return _flip; }
	void SetFlip(bool value) { _flip = value; }

	glm::vec2 SurfaceToContent(glm::vec2 value) const;
	glm::vec2 ContentToSurface(glm::vec2 value) const;

	virtual glm::mat4 GetContentTransform() const;

	virtual void Render(const glm::mat4& transform) = 0;
	virtual void Update(double secondsSinceLastUpdate) = 0;

private:
	void SetFrameValue(const bounds2f& value);
};


#endif
