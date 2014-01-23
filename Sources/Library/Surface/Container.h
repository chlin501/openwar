#ifndef Container_H
#define Container_H

#include <vector>
#include "Content.h"


class Container : public Content
{
	friend void SetContentContainer(Content*, Container*);

	std::vector<Content*> _contents;

public:
	Container();
	virtual ~Container();

	virtual void AddContent(Content* content);
	virtual void RemoveContent(Content* content);
	virtual bool HasContent(Content* content);

	virtual void Render(const glm::mat4& transform);
};


void SetContentContainer(Content* content, Container* container);


#endif
