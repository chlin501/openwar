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


WidgetShape* Widget::GetWidgetShape() const
{
	return _widgetShape;
}
