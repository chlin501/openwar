#include "Layout.h"


/* LayoutPoint */


LayoutPoint::LayoutPoint()
{
}


glm::vec2 LayoutPoint::GetValue() const
{
	return _value;
}


void LayoutPoint::SetValue(const glm::vec2& value)
{
	_value = value;
}


/* LayoutSize */


LayoutSize::LayoutSize()
{
}


glm::vec2 LayoutSize::GetValue() const
{
	return _value;
}


void LayoutSize::SetValue(const glm::vec2& value)
{
	_value = value;
}


/* LayoutBounds */


LayoutBounds::LayoutBounds()
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
