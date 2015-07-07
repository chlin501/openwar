#ifndef ScrollerView_H
#define ScrollerView_H

#include "View.h"
#include "Graphics/Viewport.h"
#include "ScrollerHotspot.h"


class ScrollerView : public View
{
	std::shared_ptr<ScrollerViewport> _scrollerViewport;
	ScrollerHotspot _scrollerHotspot;

public:
	ScrollerView(ViewOwner* viewOwner, std::shared_ptr<ScrollerViewport> viewport);

	bounds2f GetCoverage() const;

public: // View
	void OnTouchEnter(Touch* touch) override;
	void OnTouchBegin(Touch* touch) override;

	void Render() override;
};


#endif
