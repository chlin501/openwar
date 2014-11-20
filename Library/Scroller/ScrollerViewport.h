// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ScrollerViewport_H
#define ScrollerViewport_H

#include "Viewport.h"


class ScrollerViewport : public Viewport
{
	glm::vec2 _contentOffset;
	glm::vec2 _contentSize;

public:
	ScrollerViewport(GraphicsContext* gc);
	virtual ~ScrollerViewport();

	virtual glm::mat4 GetTransform() const;

	glm::vec2 GetContentOffset() const;
	void SetContentOffset(glm::vec2 value);

	glm::vec2 GetClampedOffset(glm::vec2 value) const;
	void ClampContentOffset();

	glm::vec2 GetContentSize() const;
	void SetContentSize(glm::vec2 value);

	bounds2f GetVisibleBounds() const;

	glm::vec2 LocalToGlobal(glm::vec2 value) const;
	glm::vec2 GlobalToLocal(glm::vec2 value) const;

	glm::vec2 LocalToNormalized(glm::vec2 value) const;
	glm::vec2 NormalizedToLocal(glm::vec2 value) const;
};



#endif
