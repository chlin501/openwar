#include "Layout.h"


/* LayoutContext */


LayoutContext::~LayoutContext()
{

}


/* LayoutBounds */


LayoutBounds::LayoutBounds(LayoutContext* context) :
	_context(context)
{
}


bounds2f LayoutBounds::GetValue() const
{
	return _value;
}


void LayoutBounds::SetValue(const bounds2f& value)
{
	_value = value;
}
