#ifndef ButtonWidget_H
#define ButtonWidget_H

#include "Widget.h"
#include "ImageWidget.h"
#include "StringWidget.h"

class ButtonHotspot;


struct ButtonWidget
{
	std::shared_ptr<ButtonHotspot> hotspot;
	bounds2f bounds;
	ImageWidget borderImage;
	ImageWidget iconImage;
	StringWidget titleString;
};


#endif
