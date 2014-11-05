// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonsView_H
#define ButtonsView_H

#include <string>
#include "Algebra/bounds.h"
#include "Texture.h"
#include "Content.h"
#include "ButtonRendering.h"
#include "ButtonHotspot.h"


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


/*struct TextureImageX
{
	TextureAtlas* _textureAtlas;
	bounds2f _outer_uv;
	bounds2f _outer_bounds;

	TextureImageX() : _textureAtlas(nullptr) { }
	TextureImageX(TextureAtlas* t, glm::vec2 s, bounds2f b) : _textureAtlas(t), _outer_uv(b), _outer_bounds(0, 0, s) { }

	bounds2f GetOuterBounds() const { return _outer_bounds; }
	bounds2f GetOuterUV() const { return _outer_uv; }
};*/



class ButtonItem
{
	std::shared_ptr<ButtonHotspot> _hotspot;
	ButtonArea* _buttonArea;
	std::string _buttonText;
	TextureImage* _buttonIcon;
	char _keyboardShortcut;
	bounds2f _bounds;
	bool _selected;
	bool _disabled;

public:
	PatchGlyph selectedGlyph;
	PatchGlyph buttonIconGlyph;
	PatchGlyph highlightGlyph;
	StringGlyph buttonTextGlyph;

	ButtonItem(ButtonArea* buttonArea, const char* text);
	ButtonItem(ButtonArea* buttonArea, TextureImage* icon);
	~ButtonItem();

	std::shared_ptr<ButtonHotspot> GetHotspot() const { return _hotspot; }
	ButtonArea* GetButtonArea() const { return _buttonArea; }

	const char* GetButtonText() const { return _buttonText.empty() ? nullptr : _buttonText.c_str(); }
	void SetButtonText(const char* value);

	TextureImage* GetButtonIcon() const { return _buttonIcon; }
	void SetButtonIcon(TextureImage* value) { _buttonIcon = value; }

	ButtonItem* SetAction(std::function<void()> action) { _hotspot->SetAction(action); return this; }
	bool HasAction() const { return (bool)_hotspot->GetAction(); }
	void CallAction() const { _hotspot->GetAction()(); }

	char GetKeyboardShortcut() const { return _keyboardShortcut; }
	void SetKeyboardShortcut(char value) { _keyboardShortcut = value; }

	bounds2f GetBounds() const { return _bounds; }
	void SetBounds(bounds2f value) { _bounds = value; }

	bool IsHighlight() const { return _hotspot->IsHighlight(); }
	void SetHighlight(bool value) { _hotspot->SetHighlight(value); }

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

	PatchGlyph backgroundGlyph;

	ButtonArea(ButtonGrid* buttonView, int numberOfColumns);
	~ButtonArea();

	glm::vec2 GetMargin() const { return _margin; }
	void SetMargin(glm::vec2 value) { _margin = value; }

	ButtonGrid* GetButtonView() const { return _buttonView; }

	ButtonItem* AddButtonItem(const char* buttonText);
	ButtonItem* AddButtonItem(TextureImage* buttonIcon);

	glm::vec2 CalculateSize() const;

	void UpdateColumnsAndRows();
	void UpdateBounds(bounds2f bounds);
};


class ButtonGrid : public Content
{
public:
	ButtonRendering* _buttonRendering;

private:
	ButtonAlignment _alignment;
	std::vector<ButtonArea*> _buttonAreas;
	std::vector<ButtonArea*> _obsolete;

public:
	ButtonGrid(GraphicsContext* gc, ButtonRendering* buttonRendering, ButtonAlignment alignment);

	ButtonAlignment GetAlignment() const { return _alignment; }

	virtual void SetFrame(bounds2f value);

	const std::vector<ButtonArea*>& GetButtonAreas() const {  return _buttonAreas; }

	bool HasButtons() const;
	void Reset();

	ButtonArea* AddButtonArea(int numberOfColumns = 1);

	void UpdateLayout();

	virtual void Update(double secondsSinceLastUpdate);
	virtual void Render();
	virtual void FindHotspots(glm::vec2 viewportPosition, Touch* touch);
};


#endif
