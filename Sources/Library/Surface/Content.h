// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef CONTENT_H
#define CONTENT_H

#include <functional>
#include "bounds.h"

class Container;
class Hotspot;
class Surface;


glm::mat4 ViewportTransform(bounds2f viewport, glm::vec2 translate = glm::vec2(), float rotate = 0);


class Content
{
	friend void SetContentContainer(Content*, Container*, Content*);

	Surface* _surface;
	Container* _container;
	bool _visible;
	bounds2f _frame;
	glm::vec2 _anchor;
	bounds2f _bounds;
	float _rotate;
	glm::vec2 _scale;
	glm::vec2 _translate;
	glm::mat4 _transform;
	bool _flip;

public:
	Content();
	virtual ~Content();

	Surface* GetSurface() const { return _surface; }

	virtual Container* GetContainer() const;
	virtual void SetContainer(Container* value, Content* behindContent = nullptr);


	//

	virtual bool IsVisible() const;
	void SetVisible(bool value);

	//

	virtual bounds2f GetFrame() const;
	virtual void SetFrame(bounds2f value);
	virtual void OnFrameChanged();

	virtual glm::vec2 GetAnchor() const;
	virtual void SetAnchor(glm::vec2 value);

	virtual glm::vec2 GetPosition() const;
	virtual void SetPosition(glm::vec2 value);

	virtual glm::vec2 GetSize() const;
	virtual void SetSize(glm::vec2 value);

	//

	virtual bounds2f GetBounds() const;
	virtual void SetBounds(const bounds2f& value);

	//

	virtual float GetRotate() const;
	virtual void SetRotate(float value);

	virtual glm::vec2 GetScale() const;
	virtual void SetScale(glm::vec2 value);

	virtual glm::vec2 GetTranslate() const;
	virtual void SetTranslate(glm::vec2 value);

	virtual glm::mat4 GetTransform() const;
	virtual void SetTransform(const glm::mat4& value);

	virtual glm::mat4 GetContentTransform() const;




	virtual void UseViewport();

	bool GetFlip() const { return _flip; }
	void SetFlip(bool value) { _flip = value; }

	glm::vec2 SurfaceToContent(glm::vec2 value) const;
	glm::vec2 ContentToSurface(glm::vec2 value) const;

	virtual void Update(double secondsSinceLastUpdate) = 0;
	virtual void Render(const glm::mat4& transform) = 0;
	virtual void FindHotspots(const glm::mat4 transform, glm::vec2 position, std::function<void(Hotspot*)> action) = 0;

private:
	void SetFrameValue(const bounds2f& value);

protected:
	void RenderSolid(const glm::mat4& transform, bounds2f bounds, glm::vec4 color) const;
	void RenderOutline(const glm::mat4& transform, bounds2f bounds, glm::vec4 color) const;
};


#endif
