#include "InputHotspot.h"
#include "InputWidget.h"


InputHotspot::InputHotspot(InputWidget* inputWidget) :
	_inputWidget(inputWidget),
	_inputGesture(this)
{
}


InputHotspot::~InputHotspot()
{
}


Gesture* InputHotspot::GetGesture()
{
	return &_inputGesture;
}


InputWidget* InputHotspot::GetInputWidget() const
{
	return _inputWidget;
}
