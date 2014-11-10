#include "RenderLoopObserver.h"


std::vector<RenderLoopObserver*> RenderLoopObserver::_instances;
std::vector<RenderLoopObserver*> RenderLoopObserver::_snapshot;


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
	_snapshot = _instances;
	for (RenderLoopObserver* timer : _snapshot)
		timer->OnRenderLoop(secondsSinceLastLoop);
}
