// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Widget.h"
#include "WidgetView.h"


Widget::Widget()
	: _widgetView(nullptr)
{
}


Widget::~Widget()
{
	if (_widgetView != nullptr)
		_widgetView->RemoveWidget(this);
}
