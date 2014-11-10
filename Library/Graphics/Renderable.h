#ifndef Renderable_H
#define Renderable_H


class Renderable
{
public:
	virtual ~Renderable();

	virtual void Render() = 0;
	virtual bool HasChangedSinceLastRender() const = 0;
};


#endif
