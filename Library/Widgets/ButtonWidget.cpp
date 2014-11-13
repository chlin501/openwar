#include "ButtonWidget.h"
#include "ButtonHotspot.h"


ButtonWidget::ButtonWidget() :
	_disabled(false)
{

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


bounds2f ButtonWidget::GetBounds() const
{
	return _bounds;
}


void ButtonWidget::SetBounds(const bounds2f& value)
{
	_bounds = value;
}


BorderInset ButtonWidget::GetInset() const
{
	return _inset;
}


void ButtonWidget::SetInset(const BorderInset& value)
{
	_inset = value;
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


void ButtonWidget::AppendVertices(WidgetShape* widgetShape, std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
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

	_borderImage.SetBounds(BorderBounds(_bounds).Inset(_inset));
	_borderImage.AppendVertices(widgetShape, vertices);

	_iconImage.SetBounds(BorderBounds(_bounds).Inset(_inset));
	_iconImage.AppendVertices(widgetShape, vertices);

	_titleString.SetPosition(_bounds.center() - 0.5f * widgetShape->MeasureStringWidget(&_titleString));
	_titleString.SetGlow(glm::vec4(0, 0, 0, 1));
	_titleString.SetAlpha(_disabled ? 0.5f : 1.0f);
	_titleString.AppendVertices(widgetShape, vertices);
}
