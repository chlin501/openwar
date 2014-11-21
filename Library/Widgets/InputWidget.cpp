#include "InputWidget.h"
#include "InputAdapter.h"
#include "InputHotspot.h"
#include "ScrollerViewport.h"
#include "Surface.h"
#include "Touch.h"



InputWidget::InputWidget(WidgetOwner* widgetOwner) : StringWidget(widgetOwner),
	_inputAdater(nullptr)
{
}


InputWidget::~InputWidget()
{
}


glm::vec2 InputWidget::GetPosition() const
{
	return StringWidget::GetPosition();
}


void InputWidget::SetPosition(glm::vec2 value)
{
	StringWidget::SetPosition(value);
}


const float InputWidget::GetWidth() const
{
	return StringWidget::GetWidth();
}


void InputWidget::SetWidth(float value)
{
	StringWidget::SetWidth(value);
}


bounds2f InputWidget::GetBounds() const
{
	glm::vec2 p = GetPosition();
	glm::vec2 s = MeasureSize();
	return bounds2f(p, p + s);
}


void InputWidget::SetBounds(const bounds2f& value)
{
	SetPosition(value.min);
}


void InputWidget::RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
	StringWidget::RenderVertices(vertices);
}


void InputWidget::OnTouchBegin(Touch* touch)
{
	if (GetBounds().contains(GetViewport()->GlobalToLocal(touch->GetCurrentPosition())))
	{
		if (_inputHotspot == nullptr)
			_inputHotspot = std::make_shared<InputHotspot>(this);
		_inputHotspot->SubscribeTouch(touch);
	}
}


void InputWidget::ShowInputAdapter()
{
	if (_inputAdater == nullptr)
	{
#ifdef ENABLE_SURFACE_ADAPTER_MAC
		_inputAdater = new InputAdapterMac(this);
#endif
	}
	else
	{
		_inputAdater->OnInputWidgetChanged();
	}
}
