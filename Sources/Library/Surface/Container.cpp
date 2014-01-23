#include "Container.h"


Container::Container()
{
}


Container::~Container()
{
}


void Container::AddContent(Content* content)
{
	SetContentContainer(content, this);
}


void Container::RemoveContent(Content* content)
{
	SetContentContainer(content, nullptr);
}


bool Container::HasContent(Content* content)
{
	for (Content* c : _contents)
		if (c == content)
			return true;
	return false;
}


void Container::Render(const glm::mat4& transform)
{

}


void SetContentContainer(Content* content, Container* container)
{
	Container* existing = content->GetContainer();
	if (existing != container)
	{
		if (existing != nullptr)
		{
			for (auto i = existing->_contents.begin(); i != existing->_contents.end(); ++i)
				if (*i == content)
				{
					existing->_contents.erase(i);
					break;
				}
		}

		content->_container = container;

		if (container != nullptr)
		{
			container->_contents.push_back(content);
		}
	}
}
