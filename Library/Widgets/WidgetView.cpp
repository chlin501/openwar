#include "WidgetView.h"
#import "GraphicsContext.h"
#import "WidgetShape.h"


WidgetView::WidgetView(GraphicsContext* gc) :
	_gc(gc),
	_widgetShape(nullptr)
{
}


WidgetView::~WidgetView()
{
}


/*GraphicsContext* WidgetView::GetGraphicsContext() const
{
	return _gc;
}*/


WidgetShape* WidgetView::GetWidgetShape() const
{
	if (_widgetShape == nullptr)
		_widgetShape = new WidgetShape(_gc->GetWidgetTextureAtlas());

	return _widgetShape;
}
