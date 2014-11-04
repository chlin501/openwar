// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonGrid.h"
#include "Touch.h"


const ButtonAlignment ButtonAlignment::TopLeft(ButtonAlignment::Vertical::Top, ButtonAlignment::Horizontal::Left);
const ButtonAlignment ButtonAlignment::TopCenter(ButtonAlignment::Vertical::Top, ButtonAlignment::Horizontal::Center);
const ButtonAlignment ButtonAlignment::TopRight(ButtonAlignment::Vertical::Top, ButtonAlignment::Horizontal::Right);
const ButtonAlignment ButtonAlignment::CenterLeft(ButtonAlignment::Vertical::Center, ButtonAlignment::Horizontal::Left);
const ButtonAlignment ButtonAlignment::Center(ButtonAlignment::Vertical::Center, ButtonAlignment::Horizontal::Center);
const ButtonAlignment ButtonAlignment::CenterRight(ButtonAlignment::Vertical::Center, ButtonAlignment::Horizontal::Right);
const ButtonAlignment ButtonAlignment::BottomLeft(ButtonAlignment::Vertical::Bottom, ButtonAlignment::Horizontal::Left);
const ButtonAlignment ButtonAlignment::BottomCenter(ButtonAlignment::Vertical::Bottom, ButtonAlignment::Horizontal::Center);
const ButtonAlignment ButtonAlignment::BottomRight(ButtonAlignment::Vertical::Bottom, ButtonAlignment::Horizontal::Right);


/***/


ButtonItem::ButtonItem(ButtonArea* buttonArea, const char* text) :
_hotspot(),
_buttonArea(buttonArea),
_buttonText(text),
_buttonIcon(nullptr),
_keyboardShortcut('\0'),
_selected(false),
_disabled(false)
{
	_hotspot = std::make_shared<ButtonHotspot>();
}



ButtonItem::ButtonItem(ButtonArea* buttonArea, ButtonIcon* icon) :
_hotspot(),
_buttonArea(buttonArea),
_buttonText(),
_buttonIcon(icon),
_keyboardShortcut('\0'),
_selected(false),
_disabled(false)
{
	_hotspot = std::make_shared<ButtonHotspot>();
}



ButtonItem::~ButtonItem()
{
}


void ButtonItem::SetButtonText(const char* value)
{
	_buttonText = value != nullptr ? value : "";
	_buttonArea->GetButtonView()->UpdateLayout();
}


glm::vec2 ButtonItem::CalculateSize() const
{
	if (!_buttonText.empty())
	{
		glm::vec2 size = _buttonArea->GetButtonView()->_buttonRendering->_string_font->MeasureText(_buttonText.c_str());
		return glm::vec2(38 + size.x, fmaxf(44, size.y));
	}

	if (_buttonIcon != nullptr)
		if (_buttonIcon->size.x > 44 || _buttonIcon->size.y > 44)
			return _buttonIcon->size * _buttonIcon->scale;

	return glm::vec2(44, 44);
}



/***/



ButtonArea::ButtonArea(ButtonGrid* buttonView, int numberOfColumns) :
_buttonView(buttonView),
noaction([](){})
{
	if (numberOfColumns > 0)
		columns.assign(numberOfColumns, 0);
}



ButtonArea::~ButtonArea()
{
	for (ButtonItem* buttonItem : buttonItems)
		delete buttonItem;
}



ButtonItem* ButtonArea::AddButtonItem(const char* buttonText)
{
	ButtonItem* buttonItem = new ButtonItem(this, buttonText);

	buttonItems.push_back(buttonItem);

	_buttonView->UpdateLayout();

	return buttonItem;
}



ButtonItem* ButtonArea::AddButtonItem(ButtonIcon* buttonIcon)
{
	ButtonItem* buttonItem = new ButtonItem(this, buttonIcon);
	buttonItems.push_back(buttonItem);

	_buttonView->UpdateLayout();

	return buttonItem;
}



glm::vec2 ButtonArea::CalculateSize() const
{
	glm::vec2 result;

	for (float width : columns)
		result.x += width;

	for (float height : rows)
		result.y += height;

	return result;
}



void ButtonArea::UpdateColumnsAndRows()
{
	if (buttonItems.empty())
		return;

	int numberOfColumns = columns.empty() ? 1 : (int)columns.size();
	int numberOfRows = (int)(buttonItems.size() + columns.size() - 1) / (int)columns.size();

	columns.assign(numberOfColumns, 0);
	rows.assign(numberOfRows, 0);

	for (size_t i = 0; i < rows.size(); ++i)
		for (size_t j = 0; j < columns.size(); ++j)
		{
			size_t index = i * columns.size() + j;
			if (index < buttonItems.size())
			{
				glm::vec2 size = buttonItems[index]->CalculateSize();
				columns[j] = fmaxf(columns[j], size.x);
				rows[i] = fmaxf(rows[i], size.y);
			}
		}

	if (buttonItems.size() == 1 && buttonItems.front()->GetButtonText() != nullptr)
	{
		columns.front() = fmaxf(172, columns.front());
	}
}



void ButtonArea::UpdateBounds(bounds2f bounds)
{
	_bounds = bounds.grow(_margin);

	float y = 0;
	for (size_t i = 0; i < rows.size(); ++i)
	{
		float h = rows[i];
		float x = 0;
		for (size_t j = 0; j < columns.size(); ++j)
		{
			float w = columns[j];
			size_t index = i * columns.size() + j;
			if (index < buttonItems.size())
			{
				buttonItems[index]->SetBounds(bounds2f(x, -y - h, x + w, -y) + bounds.p12());
			}
			x += w;
		}
		y += h;
	}
}



/***/




ButtonGrid::ButtonGrid(ButtonRendering* buttonRendering, ButtonAlignment alignment) :
_buttonRendering(buttonRendering),
_alignment(alignment)
{
}



void ButtonGrid::SetFrame(bounds2f value)
{
	Content::SetFrame(value);
	UpdateLayout();
}


bool ButtonGrid::HasButtons() const
{
	return !_buttonAreas.empty() && !_buttonAreas.front()->buttonItems.empty();
}



void ButtonGrid::Reset()
{
	_obsolete.insert(_obsolete.end(), _buttonAreas.begin(), _buttonAreas.end());
	_buttonAreas.clear();
}



ButtonArea* ButtonGrid::AddButtonArea(int numberOfColumns)
{
	ButtonArea* buttonArea = new ButtonArea(this, numberOfColumns);
	_buttonAreas.push_back(buttonArea);
	return buttonArea;
}



void ButtonGrid::UpdateLayout()
{
	bounds2f viewport = GetFrame();
	glm::vec2 viewport_center = viewport.center();
	float margin = 3;
	float spacing = 20;
	std::vector<bounds2f> bounds;
	glm::vec2 position(-spacing, spacing);

	for (ButtonArea* buttonArea : _buttonAreas)
	{
		buttonArea->UpdateColumnsAndRows();
		glm::vec2 size = buttonArea->CalculateSize();
		position += glm::vec2(spacing, -spacing - size.y);
		bounds.push_back(bounds2_from_corner(position, size));
		position += glm::vec2(size.x, 0);
	}

	if (_alignment.horizontal == ButtonAlignment::Horizontal::Center)
	{
		switch (_alignment.vertical)
		{
			case ButtonAlignment::Vertical::Top:
				for (bounds2f& b : bounds)
					b += glm::vec2(viewport_center.x - b.x().center(), viewport.max.y - margin);
				break;

			case ButtonAlignment::Vertical::Center:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(viewport_center.x - b.x().center(), viewport_center.y - position.y / 2);
				break;

			case ButtonAlignment::Vertical::Bottom:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(viewport_center.x - b.x().center(), viewport.min.y + margin - position.y);
				break;
		}
	}
	else
	{
		switch (_alignment.horizontal)
		{
			case ButtonAlignment::Horizontal::Left:
				for (bounds2f& b : bounds)
					b += glm::vec2(viewport.min.x + margin, 0);
				break;

			case ButtonAlignment::Horizontal::Right:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(viewport.max.x - margin - position.x, 0);
				break;

			default:
				break;
		}

		switch (_alignment.vertical)
		{
			case ButtonAlignment::Vertical::Top:
				for (bounds2f& b : bounds)
					b += glm::vec2(0, viewport.max.y - margin - b.max.y);
				break;

			case ButtonAlignment::Vertical::Center:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(0, viewport_center.y - b.y().center());
				break;

			case ButtonAlignment::Vertical::Bottom:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(0, viewport.min.y + margin - b.min.y);
				break;
		}
	}

	for (size_t i = 0; i < _buttonAreas.size(); ++i)
		_buttonAreas[i]->UpdateBounds(bounds[i]);
}


void ButtonGrid::Update(double secondsSinceLastUpdate)
{
	if (!_obsolete.empty())
	{
		for (ButtonArea* buttonArea : _obsolete)
			delete buttonArea;
		_obsolete.clear();
	}
}


void ButtonGrid::Render(const glm::mat4& transform)
{
	for (ButtonArea* buttonArea : _buttonAreas)
	{
		_buttonRendering->RenderBackground(transform, buttonArea->_bounds);

		for (ButtonItem* buttonItem : buttonArea->buttonItems)
		{
			if (buttonItem->IsSelected())
				_buttonRendering->RenderSelected(transform, buttonItem->GetBounds());

			if (buttonItem->GetButtonIcon() != nullptr)
				_buttonRendering->RenderButtonIcon(transform, buttonItem->GetBounds().center(), buttonItem->GetButtonIcon(), buttonItem->IsDisabled());

			if (buttonItem->IsHighlight())
				_buttonRendering->RenderHighlight(transform, buttonItem->GetBounds());

			if (buttonItem->GetButtonText() != nullptr)
				_buttonRendering->RenderButtonText(transform, buttonItem->GetBounds().center(), buttonItem->GetButtonText());
		}
	}
}


void ButtonGrid::FindHotspots(const glm::mat4 transform, glm::vec2 position, Touch* touch)
{
	for (ButtonArea* buttonArea : _buttonAreas)
		for (ButtonItem* buttonItem : buttonArea->buttonItems)
			if (buttonItem->HasAction()
				&& !buttonItem->IsDisabled()
				&& buttonItem->GetBounds().contains(position))
			{
				touch->AddHotspot(buttonItem->GetHotspot());
			}
}
