#include "Container.h"
#include "Surface.h"
#include "GraphicsOpenGL.h"


Container::Container(GraphicsContext* gc) : Content(gc)
{
}


Container::~Container()
{
}


void Container::AddContent(Content* content, Content* belowContent)
{
	SetContentContainer(content, this, belowContent);
}


void Container::RemoveContent(Content* content)
{
	SetContentContainer(content, nullptr, nullptr);
}


bool Container::HasContent(Content* content)
{
	for (Content* c : _contents)
		if (c == content)
			return true;
	return false;
}


Content* Container::GetBackContent() const
{
	return _contents.empty() ? nullptr : _contents.front();
}


Content* Container::GetFrontContent() const
{
	return _contents.empty() ? nullptr : _contents.back();
}


void SetContentContainer(Content* content, Container* container, Content* behindContent)
{
	Container* existing = content->GetContainer();
	if (existing != nullptr)
	{
		auto i = std::find(existing->_contents.begin(), existing->_contents.end(), content);
		existing->_contents.erase(i);
	}

	content->_container = container;

	if (container != nullptr)
	{
		auto i = std::find(container->_contents.begin(), container->_contents.end(), behindContent);
		container->_contents.insert(i, content);
	}
}
