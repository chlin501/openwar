#include "InputGesture.h"
#include "InputHotspot.h"
#include "InputWidget.h"
#include "ScrollerViewport.h"
#include "Touch.h"


InputGesture::InputGesture(InputHotspot* inputHotspot) :
	_inputHotspot(inputHotspot)
{
}


InputGesture::~InputGesture()
{
}


void InputGesture::TouchBegan(Touch* touch)
{
	InputWidget* inputWidget = _inputHotspot->GetInputWidget();
	glm::vec2 p = inputWidget->GetViewport()->GlobalToLocal(touch->GetCurrentPosition());
	if (inputWidget->GetBounds().contains(p) &&_inputHotspot->TryCaptureTouch(touch))
	{
		inputWidget->ShowInputAdapter();
	}
}


void InputGesture::TouchEnded(Touch* touch)
{
	if (_inputHotspot->HasCapturedTouch(touch))
		_inputHotspot->ReleaseTouch(touch);
}
