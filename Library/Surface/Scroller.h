#ifndef Scroller_H
#define Scroller_H

#include "Container.h"


class Scroller : public Container
{
	glm::vec2 _contentSize;
	glm::vec2 _contentOffset;
	float _zoomScale;
	bool _verticalScrolling;
	bool _horizontalScrolling;

public:
	Property<glm::vec2> contentSize;
	Property<glm::vec2> contentOffset;
	Property<float> zoomScale;

public:
	Scroller(GraphicsContext* gc);
	virtual ~Scroller();

	virtual glm::vec2 GetContentSize() const;
	virtual void SetContentSize(glm::vec2 value);

	virtual glm::vec2 GetContentOffset() const;
	virtual void SetContentOffset(glm::vec2 value);

	virtual float GetZoomScale() const;
	virtual void SetZoomScale(float value);

	virtual bool GetVerticalScrolling() const;
	virtual void SetVerticalScrolling(bool value);

	virtual bool GetHorizontalScrolling() const;
	virtual void SetHorizontalScrolling(bool value);


	// Content

	virtual void FindHotspots(Touch* touch);

};



#endif
