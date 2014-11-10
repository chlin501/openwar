// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef CONTENT_H
#define CONTENT_H

#include <functional>
#include <memory>
#include "Algebra/bounds.h"
#include "Property.h"

class Container;
class GraphicsContext;
class Hotspot;
class Touch;
class WidgetShape;


class Content
{
	friend void SetContentContainer(Content*, Container*, Content*);

	GraphicsContext* _gc;
	Container* _container;
	bool _visible;

	bounds2i _viewportBounds;
	glm::vec2 _contentOffset;
	glm::vec2 _contentSize;

	bool _flip;
	bool _dismissed;

public:
	Content(GraphicsContext* gc);
	virtual ~Content();

	virtual void Dismiss();
	virtual bool IsDismissed() const;

	GraphicsContext* GetGraphicsContext() const { return _gc; }

	virtual Container* GetContainer() const;
	virtual void SetContainer(Container* value, Content* behindContent = nullptr);

	virtual bool IsVisible() const;
	void SetVisible(bool value);

	virtual bounds2i GetViewportBounds() const;
	virtual void SetViewportBounds(bounds2i value);
	virtual void UseViewport();

	glm::vec2 GetContentOffset() const;
	void SetContentOffset(glm::vec2 value);
	glm::vec2 ClampContentOffset(glm::vec2 value) const;

	glm::vec2 GetContentSize() const;
	void SetContentSize(glm::vec2 value);

	virtual bounds2f GetVisibleBounds() const;

	virtual glm::mat4 GetRenderTransform() const;

	bool GetFlip() const { return _flip; }
	void SetFlip(bool value) { _flip = value; }

	glm::vec2 LocalToGlobal(glm::vec2 value) const;
	glm::vec2 GlobalToLocal(glm::vec2 value) const;

	glm::vec2 LocalToNormalized(glm::vec2 value) const;
	glm::vec2 NormalizedToLocal(glm::vec2 value) const;

	glm::vec2 GlobalToNormalized(glm::vec2 value) const;
	glm::vec2 NormalizedToGlobal(glm::vec2 value) const;

	virtual void Update(double secondsSinceLastUpdate) = 0;
	virtual void FindHotspots(Touch* touch) = 0;
};


#endif
