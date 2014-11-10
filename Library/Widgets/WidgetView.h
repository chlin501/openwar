#ifndef WidgetView_H
#define WidgetView_H

class GraphicsContext;
class WidgetShape;


class WidgetView
{
	GraphicsContext* _gc;
	mutable WidgetShape* _widgetShape;

public:
	WidgetView(GraphicsContext* gc);
	~WidgetView();

	//GraphicsContext* GetGraphicsContext() const;
	WidgetShape* GetWidgetShape() const;

};



#endif
