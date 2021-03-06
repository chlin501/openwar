// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonsView_H
#define ButtonsView_H

#include <string>
#include "Algebra/bounds.h"
#include "Graphics/Texture.h"
#include "WidgetView.h"
#include "ButtonGridTextureSheet.h"
#include "Surface/ClickHotspot.h"
#include "Surface/Animation.h"
#include "ImageWidget.h"
#include "StringWidget.h"

class ButtonArea;
class ButtonGrid;


struct ButtonAlignment
{
	enum class Vertical { Top, Center, Bottom };
	enum class Horizontal { Left, Center, Right };

	static const ButtonAlignment TopLeft;
	static const ButtonAlignment TopCenter;
	static const ButtonAlignment TopRight;
	static const ButtonAlignment CenterLeft;
	static const ButtonAlignment Center;
	static const ButtonAlignment CenterRight;
	static const ButtonAlignment BottomLeft;
	static const ButtonAlignment BottomCenter;
	static const ButtonAlignment BottomRight;

	Vertical vertical;
	Horizontal horizontal;

	ButtonAlignment() : vertical(Vertical::Center), horizontal(Horizontal::Center) { }
	ButtonAlignment(Vertical v, Horizontal h) : vertical(v), horizontal(h) { }

	bool operator==(const ButtonAlignment& other) const { return vertical == other.vertical && horizontal == other.horizontal; }
	bool operator!=(const ButtonAlignment& other) const { return vertical != other.vertical || horizontal != other.horizontal; }
};


class ButtonItem
{
	ClickHotspot _hotspot;
	ButtonArea* _buttonArea;
	std::shared_ptr<TextureImage> _buttonIcon;
	char _keyboardShortcut;
	bounds2f _bounds;
	bool _selected;
	bool _disabled;

public:
	ImageWidget selectedImage;
	ImageWidget buttonImage;
	ImageWidget highlightImage;
	StringWidget buttonString;

	ButtonItem(ButtonArea* buttonArea, const char* text);
	ButtonItem(ButtonArea* buttonArea, std::shared_ptr<TextureImage> icon);
	~ButtonItem();

	ClickHotspot* GetHotspot() { return &_hotspot; }
	ButtonArea* GetButtonArea() const { return _buttonArea; }

	const char* GetButtonText() const { return buttonString.GetString()[0] == '\0' ? nullptr : buttonString.GetString(); }
	void SetButtonText(const char* value);

	std::shared_ptr<TextureImage> GetButtonIcon() const { return _buttonIcon; }
	void SetButtonIcon(std::shared_ptr<TextureImage> value) { _buttonIcon = value; }

	ButtonItem* SetAction(std::function<void()> action) { _hotspot.SetClickAction(action); return this; }
	bool HasAction() const { return (bool)_hotspot.GetClickAction(); }
	void CallAction() const { _hotspot.GetClickAction()(); }

	char GetKeyboardShortcut() const { return _keyboardShortcut; }
	void SetKeyboardShortcut(char value) { _keyboardShortcut = value; }

	bounds2f GetBounds() const { return _bounds; }
	void SetBounds(bounds2f value) { _bounds = value; }

	bool IsHighlight() const { return _hotspot.IsHighlight(); }
	void SetHighlight(bool value) { _hotspot.SetHighlight(value); }

	bool IsSelected() const { return _selected; }
	void SetSelected(bool value) { _selected = value; }

	bool IsDisabled() const { return _disabled; }
	void SetDisabled(bool value) { _disabled = value; }

	glm::vec2 CalculateSize() const;
};


class ButtonArea
{
	ButtonGrid* _buttonView;

public:
	std::vector<ButtonItem*> buttonItems;
	std::vector<float> columns;
	std::vector<float> rows;
	bounds2f _bounds;
	std::function<void()> noaction;
	glm::vec2 _margin;

	ImageWidget backgroundImage;

	ButtonArea(ButtonGrid* buttonView, int numberOfColumns);
	~ButtonArea();

	glm::vec2 GetMargin() const { return _margin; }
	void SetMargin(glm::vec2 value) { _margin = value; }

	ButtonGrid* GetButtonGrid() const { return _buttonView; }

	ButtonItem* AddButtonItem(const char* buttonText);
	ButtonItem* AddButtonItem(std::shared_ptr<TextureImage> buttonIcon);

	glm::vec2 CalculateSize() const;

	void UpdateColumnsAndRows();
	void UpdateBounds(bounds2f bounds);
};


class ButtonGrid : public WidgetView, AnimationHost
{
private:
	GraphicsContext* _gc;
	ButtonAlignment _alignment;
	std::vector<ButtonArea*> _buttonAreas;
	std::vector<ButtonArea*> _obsolete;

public:
	ButtonGridTextureSheet* _textureSheet;

public:
	ButtonGrid(Surface* surface, std::shared_ptr<ScrollerViewport> viewport, ButtonGridTextureSheet* textureSheet, ButtonAlignment alignment);

	ButtonAlignment GetAlignment() const { return _alignment; }

	const std::vector<ButtonArea*>& GetButtonAreas() const {  return _buttonAreas; }

	bool HasButtons() const;
	void Reset();

	ButtonArea* AddButtonArea(int numberOfColumns = 1);

	void UpdateLayout();

private: // RenderLoopObserver
	virtual void Animate(double secondsSinceLastUpdate);

public:
	virtual void OnTouchBegin(Touch* touch);

	virtual void Render();

};


#endif
