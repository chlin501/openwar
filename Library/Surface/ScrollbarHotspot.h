#ifndef ScrollbarHotspot_H
#define ScrollbarHotspot_H

#include "Hotspot.h"
#include "ScrollbarGesture.h"
#include "Viewport.h"


enum class ScrollbarDirection
{
	Horizontal,
	Vertical
};


class ScrollbarHotspot : public Hotspot
{
	ScrollbarGesture _gesture{this};
	ScrollerViewport& _viewport;
	ScrollbarDirection _direction;
	bounds2f _bounds{};

public:
	ScrollbarHotspot(ScrollerViewport& viewport, ScrollbarDirection direction);

	const ScrollerViewport& GetScrollerViewport() const { return _viewport; }
	ScrollerViewport& GetScrollerViewport() { return _viewport; }

	bounds2f GetBounds() const;
	void SetBounds(const bounds2f& value);

	bool ShowThumb() const;
	bounds2f GetThumbBounds() const;

public: // Hotspot
	Gesture* GetGesture() override;
};


#endif
