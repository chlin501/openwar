#ifndef WidgetView_H
#define WidgetView_H

#include "Renderable.h"
#include <vector>

class GraphicsContext;
class ScrollerViewport;
class WidgetShape;
class Widget;


class WidgetView : public Renderable
{
	GraphicsContext* _gc;
	mutable ScrollerViewport* _viewport;
	mutable WidgetShape* _widgetShape;

public:
	WidgetView(GraphicsContext* gc);
	~WidgetView();

	ScrollerViewport* GetViewport() const;
	WidgetShape* GetWidgetShape() const;

	virtual void Render();
	virtual bool HasChangedSinceLastRender() const;
};


#endif
