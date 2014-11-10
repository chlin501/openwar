#include "WidgetView.h"
#import "GraphicsContext.h"
#import "WidgetShape.h"
#import "ScrollerViewport.h"


WidgetView::WidgetView(GraphicsContext* gc) :
	_gc(gc),
	_viewport(nullptr),
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


ScrollerViewport* WidgetView::GetViewport() const
{
	if (_viewport == nullptr)
		_viewport = new ScrollerViewport(_gc);

	return _viewport;
}


WidgetShape* WidgetView::GetWidgetShape() const
{
	if (_widgetShape == nullptr)
		_widgetShape = new WidgetShape(_gc->GetWidgetTextureAtlas());

	return _widgetShape;
}
