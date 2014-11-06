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
	bounds2f _contentBounds;

	bool _isUsingDepth;
	bool _flip;
	bool _dismissed;
	mutable WidgetShape* _widgetShape;

public:
	Content(GraphicsContext* gc);
	virtual ~Content();

	virtual void Dismiss();
	virtual bool IsDismissed() const;

	GraphicsContext* GetGraphicsContext() const { return _gc; }

	WidgetShape* GetWidgetShape() const;

	virtual Container* GetContainer() const;
	virtual void SetContainer(Container* value, Content* behindContent = nullptr);

	virtual bool IsVisible() const;
	void SetVisible(bool value);

	virtual bounds2i GetViewportBounds() const;
	virtual void SetViewportBounds(bounds2i value);
	virtual void UseViewport();

	virtual bounds2f GetContentBounds() const;
	virtual void SetContentBounds(const bounds2f& value);

	virtual bool IsUsingDepth() const;
	virtual void SetUsingDepth(bool value);

	virtual glm::mat4 GetRenderTransform() const;

	bool GetFlip() const { return _flip; }
	void SetFlip(bool value) { _flip = value; }

	glm::vec2 ContentToViewport(glm::vec2 value) const;
	glm::vec2 ViewportToContent(glm::vec2 value) const;

	glm::vec2 ContentToNormalized(glm::vec2 value) const;
	glm::vec2 NormalizedToContent(glm::vec2 value) const;

	glm::vec2 ViewportToNormalized(glm::vec2 value) const;
	glm::vec2 NormalizedToViewport(glm::vec2 value) const;

	virtual void Update(double secondsSinceLastUpdate) = 0;
	virtual void Render() = 0;
	virtual void FindHotspots(Touch* touch) = 0;
};


#endif
