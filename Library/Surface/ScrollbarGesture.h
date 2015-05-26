#ifndef ScrollbarGesture_H
#define ScrollbarGesture_H

#include "Gesture.h"

class ScrollbarHotspot;


class ScrollbarGesture : public Gesture
{
	ScrollbarHotspot* _hotspot;

public:
	ScrollbarGesture(ScrollbarHotspot* hotspot);
};


#endif
