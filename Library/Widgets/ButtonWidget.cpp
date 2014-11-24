#include "ButtonWidget.h"
#include "ButtonHotspot.h"
#include "Surface/Touch.h"
#include "Scroller/ScrollerViewport.h"



ButtonWidget::ButtonWidget(WidgetOwner* widgetOwner) : WidgetGroup(widgetOwner),
	_borderImage(this),
	_iconImage(this),
	_titleString(this),
	_disabled(false)
{
	_hotspot = std::make_shared<ButtonHotspot>([this](glm::vec2 position) {
		glm::vec2 p = GetWidgetView()->GetScrollerViewport()->GlobalToLocal(position);
		return _bounds.contains(p);
	});

	_titleString.SetGlow(glm::vec4(0, 0, 0, 1));
}


bounds2f ButtonWidget::GetBounds() const
{
	return _bounds;
}


void ButtonWidget::SetBounds(const bounds2f& value)
{
	_bounds = value;
}


std::function<void()> ButtonWidget::GetClickAction() const
{
	return _hotspot->GetClickAction();
}


void ButtonWidget::SetClickAction(std::function<void()> value)
{
	_hotspot->SetClickAction(value);
}


void ButtonWidget::SetBackgroundNormal(std::shared_ptr<TextureImage> value)
{
	_backgroundNormal = value;
}


void ButtonWidget::SetBackgroundHighlight(std::shared_ptr<TextureImage> value)
{
	_backgroundHighlight = value;
}


void ButtonWidget::SetBackgroundDisabled(std::shared_ptr<TextureImage> value)
{
	_backgroundDisabled = value;
}


BorderInset ButtonWidget::GetInset() const
{
	return _inset;
}


void ButtonWidget::SetInset(const BorderInset& value)
{
	_inset = value;
	_borderImage.SetInset(_inset);
	_iconImage.SetInset(_inset);
}


const char* ButtonWidget::GetTitle() const
{
	return _titleString.GetString();
}


void ButtonWidget::SetTitle(const char* value)
{
	_titleString.SetString(value);
}


std::shared_ptr<TextureImage> ButtonWidget::GetIcon() const
{
	return _iconImage.GetTextureImage();
}


void ButtonWidget::SetIcon(std::shared_ptr<TextureImage> value)
{
	_iconImage.SetTextureImage(value);
}


bool ButtonWidget::IsDisabled() const
{
	return _disabled;
}


void ButtonWidget::SetDisabled(bool value)
{
	_disabled = value;
}


void ButtonWidget::OnTouchEnter(Touch* touch)
{
}


void ButtonWidget::OnTouchBegin(Touch* touch)
{
	if (_hotspot->IsInside(touch->GetCurrentPosition()))
		_hotspot->SubscribeTouch(touch);
}


void ButtonWidget::RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
	RefreshContent();
	WidgetGroup::RenderVertices(vertices);
}


void ButtonWidget::RefreshContent()
{
	_borderImage.SetBounds(_bounds);
	if (_disabled)
	{
		if (_backgroundDisabled != nullptr)
		{
			_borderImage.SetTextureImage(_backgroundDisabled);
			_borderImage.SetColorize(glm::vec4());
			_borderImage.SetAlpha(1);
		}
		else
		{
			_borderImage.SetTextureImage(_backgroundNormal);
			_borderImage.SetColorize(glm::vec4());
			_borderImage.SetAlpha(0.5f);
		}
	}
	else if (_hotspot != nullptr && _hotspot->IsHighlight())
	{
		if (_backgroundHighlight != nullptr)
		{
			_borderImage.SetTextureImage(_backgroundHighlight);
			_borderImage.SetColorize(glm::vec4());
			_borderImage.SetAlpha(1);
		}
		else
		{
			_borderImage.SetTextureImage(_backgroundNormal);
			_borderImage.SetColorize(glm::vec4(1, 1, 1, 0.5f));
			_borderImage.SetAlpha(1);
		}
	}
	else
	{
		_borderImage.SetTextureImage(_backgroundNormal);
		_borderImage.SetColorize(glm::vec4());
		_borderImage.SetAlpha(1);
	}

	_iconImage.SetBounds(_bounds);

	_titleString.SetPosition(_bounds.mid() - 0.5f * _titleString.MeasureSize());
	_titleString.SetAlpha(_disabled ? 0.5f : 1.0f);
}
