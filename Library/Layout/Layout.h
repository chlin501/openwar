#ifndef Layout_H
#define Layout_H

#include "bounds.h"


class LayoutPoint
{
public:
	glm::vec2 _value;

	LayoutPoint();

	glm::vec2 GetValue() const;
	void SetValue(const glm::vec2& value);

private:
	LayoutPoint(const LayoutPoint&) { }
	LayoutPoint& operator=(const LayoutPoint&) { return *this; }
};


class LayoutSize
{
public:
	glm::vec2 _value;

	LayoutSize();

	glm::vec2 GetValue() const;
	void SetValue(const glm::vec2& value);

private:
	LayoutSize(const LayoutSize&) { }
	LayoutSize& operator=(const LayoutSize&) { return *this; }
};


class LayoutBounds
{
public:
	bounds2f _value;

	LayoutBounds();

	bounds2f GetValue() const;
	void SetValue(const bounds2f& value);

private:
	LayoutBounds(const LayoutBounds&) { }
	LayoutBounds& operator=(const LayoutBounds&) { return *this; }
};


#endif
