// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Surface_H
#define Surface_H

#include <vector>
#include <glm/glm.hpp>

class GraphicsContext;
class Touch;
class View;
class Viewport;


class Surface
{
	friend class View;
	GraphicsContext* _gc;
	std::vector<View*> _views;
	glm::vec2 _size;

public:
	Surface(GraphicsContext* gc);
	virtual ~Surface();

	GraphicsContext* GetGraphicsContext() const;

	glm::vec2 GetSize() const;
	void SetSize(glm::vec2 value);

	void NotifyViewsOfTouchEnter(Touch* touch);
	void NotifyViewsOfTouchBegin(Touch* touch);

	virtual void Render();

private:
	Surface(const Surface&) { }
	Surface& operator=(const Surface&) { return *this; }
};


#endif
