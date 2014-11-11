#include "RenderLoopObserver.h"


std::vector<RenderLoopObserver*> RenderLoopObserver::_instances;


RenderLoopObserver::RenderLoopObserver()
{
	_instances.push_back(this);
}


RenderLoopObserver::~RenderLoopObserver()
{
	_instances.erase(
		std::remove(_instances.begin(), _instances.end(), this),
		_instances.end());

}


void RenderLoopObserver::NotifyRenderLoop(double secondsSinceLastLoop)
{
	for (size_t i = 0; i < _instances.size(); ++i)
		_instances[i]->OnRenderLoop(secondsSinceLastLoop);
}
