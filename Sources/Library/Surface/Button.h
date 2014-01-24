// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Button_H
#define Button_H

#include <functional>
#include <string>
#include "../Algebra/bounds.h"
#include "../Graphics/texture.h"
#include "Content.h"
//#include "string_shape.h"


class Button : public Content
{
private:
	//string_font* _string_font;
	//string_shape* _string_shape;

	std::string _buttonText;
	bool _highlight;
	std::function<void()> _action;

public:
	Button();

	const char* GetButtonText() const { return _buttonText.c_str(); }
	void SetButtonText(const char* value) { _buttonText = value; }

	bool IsHighlight() const { return _highlight; }
	void SetHighlight(bool value) { _highlight = value; }

	std::function<void()> GetAction() const { return _action; }
	void SetAction(std::function<void()> value) { _action = value; }

	// Content

	virtual glm::vec2 GetMaximumSize() const;
	virtual glm::vec2 GetMinimumSize() const;
	virtual glm::vec2 GetPreferredSize() const;

	virtual void Render(const glm::mat4& transform);
	virtual void Update(double secondsSinceLastUpdate);
	virtual void FindHotspots(const glm::mat4 transform, glm::vec2 position, std::function<void(Hotspot*)> action);

private:
	void RenderButtonText(bounds2f viewport, glm::vec2 position, const char* text);
};



#endif
