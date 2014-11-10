#ifndef WidgetViewport_H
#define WidgetViewport_H

#include "Viewport.h"


class WidgetViewport : public Viewport
{
	glm::vec2 _contentOffset;
	glm::vec2 _contentSize;

public:
	WidgetViewport(GraphicsContext* gc);
	virtual ~WidgetViewport();

	virtual glm::mat4 GetTransform() const;

	glm::vec2 GetContentOffset() const;
	void SetContentOffset(glm::vec2 value);
	glm::vec2 ClampContentOffset(glm::vec2 value) const;

	glm::vec2 GetContentSize() const;
	void SetContentSize(glm::vec2 value);

	bounds2f GetVisibleBounds() const;

	glm::vec2 LocalToGlobal(glm::vec2 value) const;
	glm::vec2 GlobalToLocal(glm::vec2 value) const;

	glm::vec2 LocalToNormalized(glm::vec2 value) const;
	glm::vec2 NormalizedToLocal(glm::vec2 value) const;
};



#endif