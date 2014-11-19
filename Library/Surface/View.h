#ifndef View_H
#define View_H

#include "Layout.h"

class GraphicsContext;
class Surface;
class Touch;
class Viewport;


class View
{
	friend class Surface;
	Surface* _surface;
	bounds2f _bounds;
	bool _visible;

public:
	View(Surface* surface);
	virtual ~View();

	Surface* GetSurface() const;
	GraphicsContext* GetGraphicsContext() const;

	bounds2f GetBounds() const;
	void SetBounds(const bounds2f& value);

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
