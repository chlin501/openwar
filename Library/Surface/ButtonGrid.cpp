// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonGrid.h"
#include "Surface.h"
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



ButtonItem::ButtonItem(ButtonArea* buttonArea, TextureImage* icon) :
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
	{
		glm::vec2 size = _buttonIcon->GetOuterBounds().size();
		if (size.x > 44 || size.y > 44)
			return size;
	}

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



ButtonItem* ButtonArea::AddButtonItem(TextureImage* buttonIcon)
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
				buttonItems[index]->SetBounds(bounds2f(x, -y - h, x + w, -y) + bounds.mix_01());
			}
			x += w;
		}
		y += h;
	}
}



/***/




ButtonGrid::ButtonGrid(GraphicsContext* gc, ButtonRendering* buttonRendering, ButtonAlignment alignment) : Content(gc),
_buttonRendering(buttonRendering),
_alignment(alignment)
{
}



void ButtonGrid::SetFrame(bounds2f value)
{
	Content::SetViewportBounds(value);
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
	bounds2f contentBounds = GetContentBounds();
	glm::vec2 contentCenter = contentBounds.center();
	float margin = 3;
	float spacing = 20;
	std::vector<bounds2f> bounds;
	glm::vec2 position(-spacing, spacing);

	for (ButtonArea* buttonArea : _buttonAreas)
	{
		buttonArea->UpdateColumnsAndRows();
		glm::vec2 size = buttonArea->CalculateSize();
		position += glm::vec2(spacing, -spacing - size.y);
		bounds.push_back(bounds2f(position, position + size));
		position += glm::vec2(size.x, 0);
	}

	if (_alignment.horizontal == ButtonAlignment::Horizontal::Center)
	{
		switch (_alignment.vertical)
		{
			case ButtonAlignment::Vertical::Top:
				for (bounds2f& b : bounds)
					b += glm::vec2(contentCenter.x - b.x().center(), contentBounds.max.y - margin);
				break;

			case ButtonAlignment::Vertical::Center:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(contentCenter.x - b.x().center(), contentCenter.y - position.y / 2);
				break;

			case ButtonAlignment::Vertical::Bottom:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(contentCenter.x - b.x().center(), contentBounds.min.y + margin - position.y);
				break;
		}
	}
	else
	{
		switch (_alignment.horizontal)
		{
			case ButtonAlignment::Horizontal::Left:
				for (bounds2f& b : bounds)
					b += glm::vec2(contentBounds.min.x + margin, 0);
				break;

			case ButtonAlignment::Horizontal::Right:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(contentBounds.max.x - margin - position.x, 0);
				break;

			default:
				break;
		}

		switch (_alignment.vertical)
		{
			case ButtonAlignment::Vertical::Top:
				for (bounds2f& b : bounds)
					b += glm::vec2(0, contentBounds.max.y - margin - b.max.y);
				break;

			case ButtonAlignment::Vertical::Center:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(0, contentCenter.y - b.y().center());
				break;

			case ButtonAlignment::Vertical::Bottom:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(0, contentBounds.min.y + margin - b.min.y);
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


void ButtonGrid::Render()
{
	glm::mat4 transform = GetRenderTransform();

	WidgetShape* buttonShape = new WidgetShape(_buttonRendering->_textureAtlas);

	for (ButtonArea* buttonArea : _buttonAreas)
	{
		buttonArea->backgroundGlyph.outer_xy = buttonArea->_bounds.grow(10);
		buttonArea->backgroundGlyph.inner_xy = buttonArea->_bounds.grow(-22);
		buttonArea->backgroundGlyph.outer_uv = _buttonRendering->buttonBackground->GetOuterUV();
		buttonArea->backgroundGlyph.inner_uv = _buttonRendering->buttonBackground->GetInnerUV();
		buttonShape->AddGlyph(&buttonArea->backgroundGlyph);

		for (ButtonItem* buttonItem : buttonArea->buttonItems)
		{
			if (buttonItem->IsSelected())
			{
				buttonItem->selectedGlyph.outer_xy = buttonItem->GetBounds().grow(10);
				buttonItem->selectedGlyph.inner_xy = buttonItem->GetBounds().grow(-22);
				buttonItem->selectedGlyph.outer_uv = _buttonRendering->buttonSelected->GetOuterUV();
				buttonItem->selectedGlyph.inner_uv = _buttonRendering->buttonSelected->GetInnerUV();
				buttonShape->AddGlyph(&buttonItem->selectedGlyph);
			}

			if (buttonItem->GetButtonIcon() != nullptr)
			{
				bounds2f bounds_xy = buttonItem->GetButtonIcon()->GetOuterBounds();
				bounds_xy -= bounds_xy.center();
				bounds_xy += buttonItem->GetBounds().center();

				buttonItem->buttonIconGlyph.outer_xy = bounds_xy;
				buttonItem->buttonIconGlyph.inner_xy = bounds_xy;
				buttonItem->buttonIconGlyph.outer_uv = buttonItem->GetButtonIcon()->GetOuterUV();
				buttonItem->buttonIconGlyph.inner_uv = buttonItem->GetButtonIcon()->GetInnerUV();
				buttonItem->buttonIconGlyph._alpha = buttonItem->IsDisabled() ? 0.5f : 1.0f;

				buttonShape->AddGlyph(&buttonItem->buttonIconGlyph);
			}

			if (buttonItem->IsHighlight())
			{
				buttonItem->highlightGlyph.outer_xy = buttonItem->GetBounds();
				buttonItem->highlightGlyph.inner_xy = bounds2f(buttonItem->GetBounds().center());
				buttonItem->highlightGlyph.outer_uv = _buttonRendering->buttonHighlight->GetOuterUV();
				buttonItem->highlightGlyph.inner_uv = _buttonRendering->buttonHighlight->GetInnerUV();
				buttonShape->AddGlyph(&buttonItem->highlightGlyph);
			}

			if (buttonItem->GetButtonText() != nullptr)
			{
				buttonItem->buttonTextGlyph.SetString(buttonItem->GetButtonText());
				buttonItem->buttonTextGlyph.SetTranslate(buttonItem->GetBounds().center() - 0.5f * buttonShape->MeasureGlyph(&buttonItem->buttonTextGlyph));
				buttonShape->AddGlyph(&buttonItem->buttonTextGlyph);
			}
		}
	}

	RenderCall<WidgetShader> renderCall(GetGraphicsContext());

	renderCall.SetVertices(buttonShape->GetVertices());
	renderCall.SetTexture("texture", buttonShape->GetTextureAtlas());
	renderCall.SetUniform("transform", transform);
	renderCall.SetUniform("color", glm::vec4(1, 1, 1, 1));
	renderCall.Render();

	delete buttonShape;
}


void ButtonGrid::FindHotspots(glm::vec2 viewportPosition, Touch* touch)
{
	glm::vec2 position = ViewportToContent(viewportPosition);

	for (ButtonArea* buttonArea : _buttonAreas)
		for (ButtonItem* buttonItem : buttonArea->buttonItems)
			if (buttonItem->HasAction()
				&& !buttonItem->IsDisabled()
				&& buttonItem->GetBounds().contains(position))
			{
				touch->AddHotspot(buttonItem->GetHotspot());
			}
}
