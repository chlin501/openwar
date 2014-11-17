#ifndef View_H
#define View_H

#include "Layout.h"

class Surface;
class Touch;
class Viewport;


class View
{
	friend class Surface;
	Surface* _surface;
	bool _visible;

public:
	LayoutBounds Bounds;

	View(Surface* surface);
	virtual ~View();

	Surface* GetSurface() const;

	bool IsVisible() const;
	void SetVisible(bool value);

	void OrderFront();
	void OrderFrontOf(View* view);
	void OrderBack();
	void OrderBackOf(View* view);

	virtual Viewport* GetViewport() const = 0;

	virtual void OnTouchEnter(Touch* touch) = 0;
	virtual void OnTouchBegin(Touch* touch) = 0;

	virtual void Render() = 0;
};


#endif
