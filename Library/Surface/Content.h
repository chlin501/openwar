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
class Hotspot;
class Surface;
class Touch;
class WidgetShape;

glm::mat4 ViewportTransform(bounds2i viewport, glm::vec2 translate = glm::vec2(), float rotate = 0);


class Content
{
	friend void SetContentContainer(Content*, Container*, Content*);

	mutable Surface* _surface;
	Container* _container;
	bool _visible;

	bounds2i _frame;
	bounds2f _bounds;

	bool _isUsingDepth;
	glm::vec2 _translate;
	bool _flip;
	bool _dismissed;
	mutable WidgetShape* _widgetShape;

public:
	Content();
	virtual ~Content();

	virtual void Dismiss();
	virtual bool IsDismissed() const;

	virtual Surface* GetSurface() const;

	WidgetShape* GetWidgetShape() const;

	virtual Container* GetContainer() const;
	virtual void SetContainer(Container* value, Content* behindContent = nullptr);

	virtual bool IsVisible() const;
	void SetVisible(bool value);

	virtual void UseViewport();

	virtual bounds2i GetFrame() const;
	virtual void SetFrame(bounds2i value);

	virtual bounds2f GetBounds() const;
	virtual void SetBounds(const bounds2f& value);

	virtual bool IsUsingDepth() const;
	virtual void SetUsingDepth(bool value);

	virtual glm::vec2 GetTranslate() const;
	virtual void SetTranslate(glm::vec2 value);

	virtual glm::mat4 GetRenderTransform() const;
	virtual glm::mat4 GetContentTransform() const;

	bool GetFlip() const { return _flip; }
	void SetFlip(bool value) { _flip = value; }

	glm::vec2 ConvertContentCoordinateToNormalizedDeviceCoordinate(glm::vec2 value) const;
	glm::vec2 ConvertNormalizedDeviceCoordinateToContentCoordinate(glm::vec2 value) const;

	virtual void Update(double secondsSinceLastUpdate) = 0;
	virtual void Render(const glm::mat4& transform) = 0;
	virtual void FindHotspots(const glm::mat4 transform, glm::vec2 position, Touch* touch) = 0;

protected:
	void RenderSolid(const glm::mat4& transform, bounds2f bounds, glm::vec4 color) const;
};


#endif
