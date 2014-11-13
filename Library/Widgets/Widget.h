// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Widget_H
#define Widget_H

#include "Vertex.h"
#include <vector>

class WidgetShape;


class Widget
{
	friend class WidgetShape;

	WidgetShape* _widgetShape;

public:
	Widget();
	virtual ~Widget();

	WidgetShape* GetWidgetShape() const;

protected:
	virtual void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices) = 0;

private:
	Widget(const Widget&) { }
	Widget& operator=(const Widget&) { return *this; }
};


#endif
