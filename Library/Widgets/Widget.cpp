#include "Widget.h"
#include "WidgetView.h"


Widget::Widget() :
	_widgetView(nullptr)
{
}


Widget::~Widget()
{
	if (_widgetView != nullptr)
		_widgetView->RemoveWidget(this);
}
