#ifndef RenderLoopObserver_H
#define RenderLoopObserver_H


class RenderLoopObserver
{
	static std::vector<RenderLoopObserver*> _instances;

public:
	RenderLoopObserver();
	virtual ~RenderLoopObserver();

	static void NotifyRenderLoop(double secondsSinceLastLoop);

	virtual void OnRenderLoop(double secondsSinceLastLoop) = 0;
};


#endif
