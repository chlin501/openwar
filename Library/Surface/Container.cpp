#include "Container.h"
#include "Surface.h"


Container::Container()
{
}


Container::~Container()
{
}


void Container::DeleteDismissedContent()
{
	std::vector<Content*> dismissed;

	for (Content* content : _contents)
		if (content->IsDismissed())
			dismissed.push_back(content);

	for (Content* content : dismissed)
		delete content;
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


void Container::Update(double secondsSinceLastUpdate)
{
	for (Content* content : _contents)
		content->Update(secondsSinceLastUpdate);
}


void Container::Render(const glm::mat4& transform)
{
	for (Content* content : _contents)
	{
		if (content->IsVisible())
		{
			content->UseViewport();

			if (content->IsUsingDepth())
			{
				glClear(GL_DEPTH_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}

			content->Render(content->GetViewportTransform() * content->GetContainerTransform() * content->GetContentTransform());
		}
	}
}


void Container::FindHotspots(const glm::mat4 transform, glm::vec2 position, std::function<void(std::shared_ptr<Hotspot>)> action)
{
	for (Content* content : _contents)
	{
		if (content->IsVisible())
		{
			content->FindHotspots(content->GetContainerTransform() * content->GetContentTransform(), position, action);
		}
	}
}


void SetContentContainer(Content* content, Container* container, Content* behindContent)
{
	Surface* surface = nullptr;
	for (Container* c = container; surface == nullptr && c != nullptr; c = c->GetContainer())
		surface = dynamic_cast<Surface*>(c);

	Container* existing = content->GetContainer();
	if (existing != nullptr)
	{
		auto i = std::find(existing->_contents.begin(), existing->_contents.end(), content);
		existing->_contents.erase(i);
	}

	content->_surface = surface;
	content->_container = container;

	if (container != nullptr)
	{
		auto i = std::find(container->_contents.begin(), container->_contents.end(), behindContent);
		container->_contents.insert(i, content);
	}
}
