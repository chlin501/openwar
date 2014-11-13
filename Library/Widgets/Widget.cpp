// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Widget.h"
#include "WidgetShape.h"


Widget::Widget()
	: _widgetShape(nullptr)
{
}


Widget::~Widget()
{
	if (_widgetShape != nullptr)
		_widgetShape->RemoveWidget(this);
}
