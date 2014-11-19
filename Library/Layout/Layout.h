#ifndef Layout_H
#define Layout_H

#include "bounds.h"


class LayoutContext
{
public:
	virtual ~LayoutContext();
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
