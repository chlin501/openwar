#ifndef View_H
#define View_H

#include "Algebra/bounds.h"

class GraphicsContext;
class Surface;
class Touch;
class ViewOwner;


class View
{
	friend class ViewOwner;
	ViewOwner* _viewOwner;
	bounds2f _bounds;
	bool _visible;

public:
	View(ViewOwner* viewOwner);
	virtual ~View();

	virtual ViewOwner* GetViewOwner() const;
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

	virtual void OnTouchEnter(Touch* touch) = 0;
	virtual void OnTouchBegin(Touch* touch) = 0;

	virtual void Render() = 0;
};


class ViewOwner
{
	friend class View;
	friend class ViewGroup;
	std::vector<View*> _views;

public:
	virtual ~ViewOwner();

	virtual const std::vector<View*>& GetViews();

	virtual void NotifyViewsOfTouchEnter(Touch* touch);
	virtual void NotifyViewsOfTouchBegin(Touch* touch);
	virtual void RenderViews();

private:
	virtual Surface* FindSurface() = 0;
};


class ViewGroup : public View, protected ViewOwner
{
public:
	ViewGroup(ViewOwner* viewOwner);

	virtual void OnTouchEnter(Touch* touch);
	virtual void OnTouchBegin(Touch* touch);

	virtual void Render();

private:
	virtual Surface* FindSurface();
};


#endif
