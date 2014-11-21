#ifndef InputHotspot_H
#define InputHotspot_H

#include "Hotspot.h"
#include "InputGesture.h"

class InputWidget;


class InputHotspot : public Hotspot
{
	InputWidget* _inputWidget;
	InputGesture _inputGesture;

public:
	InputHotspot(InputWidget* inputWidget);
	~InputHotspot();

	virtual Gesture* GetGesture();

	InputWidget* GetInputWidget() const;
};


#endif
