#ifndef WidgetView_H
#define WidgetView_H

class GraphicsContext;
class WidgetShape;
class ScrollerViewport;


class WidgetView
{
	GraphicsContext* _gc;
	mutable ScrollerViewport* _viewport;
	mutable WidgetShape* _widgetShape;

public:
	WidgetView(GraphicsContext* gc);
	~WidgetView();

	//GraphicsContext* GetGraphicsContext() const;

	ScrollerViewport* GetViewport() const;
	WidgetShape* GetWidgetShape() const;
};



#endif
