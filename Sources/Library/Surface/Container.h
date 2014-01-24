#ifndef Container_H
#define Container_H

#include <vector>
#include "Content.h"


class Container : public Content
{
	friend void SetContentContainer(Content*, Container*, Content*);

	std::vector<Content*> _contents;

public:
	Container();
	virtual ~Container();

	// Container

	virtual void AddContent(Content* content, Content* behindContent = nullptr);
	virtual void RemoveContent(Content* content);
	virtual bool HasContent(Content* content);

	virtual Content* GetBackContent() const;
	virtual Content* GetFrontContent() const;

	// Content

	virtual void Update(double secondsSinceLastUpdate);
	virtual void Render(const glm::mat4& transform);
};


void SetContentContainer(Content* content, Container* container, Content* behindContent = nullptr);


#endif
