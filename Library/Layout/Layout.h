#ifndef Layout_H
#define Layout_H

#include "bounds.h"
#include <functional>


class LayoutContext
{
public:
	virtual ~LayoutContext();
};


class LayoutPoint
{
	LayoutContext* _context;
public:
	glm::vec2 _value;

	LayoutPoint(LayoutContext* context);

	glm::vec2 GetValue() const;
	void SetValue(const glm::vec2& value);

private:
	LayoutPoint(const LayoutPoint&) { }
	LayoutPoint& operator=(const LayoutPoint&) { return *this; }
};


class LayoutSize
{
	LayoutContext* _context;
public:
	glm::vec2 _value;

	LayoutSize(LayoutContext* context);

	glm::vec2 GetValue() const;
	void SetValue(const glm::vec2& value);

private:
	LayoutSize(const LayoutSize&) { }
	LayoutSize& operator=(const LayoutSize&) { return *this; }
};


class LayoutBounds
{
	LayoutContext* _context;

public:
	bounds2f _value;

	LayoutBounds(LayoutContext* context);

	bounds2f GetValue() const;
	void SetValue(const bounds2f& value);

private:
	LayoutBounds(const LayoutBounds&) { }
	LayoutBounds& operator=(const LayoutBounds&) { return *this; }
};


#endif
