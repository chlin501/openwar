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
	Scroller();
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

	virtual glm::mat4 GetContentTransform() const;
	virtual void Render(const glm::mat4& transform);
	virtual void Update(double secondsSinceLastUpdate);
	virtual void FindHotspots(const glm::mat4 transform, glm::vec2 position, std::function<void(std::shared_ptr<Hotspot>)> action);

};



#endif
