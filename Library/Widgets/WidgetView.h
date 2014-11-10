#ifndef WidgetView_H
#define WidgetView_H

class GraphicsContext;
class WidgetShape;
class WidgetViewport;


class WidgetView
{
	GraphicsContext* _gc;
	mutable WidgetViewport* _viewport;
	mutable WidgetShape* _widgetShape;

public:
	WidgetView(GraphicsContext* gc);
	~WidgetView();

	//GraphicsContext* GetGraphicsContext() const;

	WidgetViewport* GetViewport() const;
	WidgetShape* GetWidgetShape() const;
};



#endif
