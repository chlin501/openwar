#ifndef View_H
#define View_H

#include "bounds.h"

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

	virtual Surface* GetSurface() const;
	virtual GraphicsContext* GetGraphicsContext() const;

	virtual bounds2f GetBounds() const;
	virtual void SetBounds(const bounds2f& value);

	virtual bool IsVisible() const;
	virtual void SetVisible(bool value);

	virtual void OrderFront();
	virtual void OrderFrontOf(View* view);
	virtual void OrderBack();
	virtual void OrderBackOf(View* view);

	virtual Viewport* GetViewport() const = 0;

	virtual void OnTouchEnter(Touch* touch) = 0;
	virtual void OnTouchBegin(Touch* touch) = 0;

	virtual void Render() = 0;
};


#endif
