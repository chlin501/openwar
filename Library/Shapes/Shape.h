#ifndef Shape_H
#define Shape_H


class Shape
{
public:
	Shape();
	virtual ~Shape();

protected:
	Shape(const Shape&) { }
	Shape& operator=(const Shape&) { return *this; }
};


#endif
