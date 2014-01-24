#include "Container.h"
#include "Surface.h"


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
	Surface* surface = nullptr;
	for (Container* c = container; surface == nullptr && c != nullptr; c = c->GetContainer())
		surface = dynamic_cast<Surface*>(c);

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

		content->_surface = surface;
		content->_container = container;

		if (container != nullptr)
		{
			container->_contents.push_back(content);
		}
	}
}
