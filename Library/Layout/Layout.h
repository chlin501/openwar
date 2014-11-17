#ifndef Layout_H
#define Layout_H

#include "bounds.h"


class LayoutContext
{
public:
	virtual ~LayoutContext();
};


class LayoutPoint
{
	LayoutContext* _context;
	glm::vec2 _value;

public:
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
	glm::vec2 _value;

public:
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
	bounds2f _value;

public:
	LayoutBounds(LayoutContext* context);

	bounds2f GetValue() const;
	void SetValue(const bounds2f& value);

private:
	LayoutBounds(const LayoutBounds&) { }
	LayoutBounds& operator=(const LayoutBounds&) { return *this; }
};


#endif
