// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Button.h"
#include "ButtonHotspot.h"
#include "Container.h"



Button::Button() : Content(),
//_string_font(nullptr),
//_string_shape(nullptr),
_highlight(false)
{
	//_string_font = new string_font(true, 18, 1);
	//_string_shape = new string_shape(_string_font);

	SetSize(GetPreferredSize());
}


/* Content */


glm::vec2 Button::GetMaximumSize() const
{
	return glm::vec2();
}


glm::vec2 Button::GetMinimumSize() const
{
	return glm::vec2();
}


glm::vec2 Button::GetPreferredSize() const
{
	return glm::vec2(100, 24);
}


void Button::Render(const glm::mat4& transform)
{
	glm::vec4 color = _highlight ? glm::vec4(1, 1, 1, 0.5f) : glm::vec4(0, 0, 0, 0.5f);
	RenderSolid(transform, GetBounds(), color);
}


void Button::Update(double secondsSinceLastUpdate)
{
}


void Button::FindHotspots(const glm::mat4 transform, glm::vec2 position, std::function<void (Hotspot*)> action)
{
	glm::vec4 p = glm::inverse(transform) * glm::vec4(position, 0, 1);

	if (GetBounds().contains(p.xy() / p.w))
		action(new ButtonHotspot(this));
}


void Button::RenderButtonText(bounds2f viewport, glm::vec2 position, const char* text)
{
	/*
	_string_shape->clear();
	_string_shape->add(text, glm::mat4x4());
	_string_shape->update(GL_STATIC_DRAW);

	string_sprite sprite(_string_font->_renderer);
	sprite._viewport = viewport;
	sprite._texture = &_string_font->_texture;
	sprite._shape = &_string_shape->_vbo;

	glm::vec2 p = position - 0.5f * _string_font->measure(text) - glm::vec2(0, 1);

	sprite._color = glm::vec4(0, 0, 0, 0.15);
	for (int dx = -1; dx <= 1; ++dx)
		for (int dy = -1; dy <= 1; ++dy)
			if (dx != 0 || dy != 0)
			{
				sprite._translate = p + glm::vec2(dx, dy);
				sprite.render();
			}

	sprite._translate = p + glm::vec2(0, -1);
	sprite._color = glm::vec4(0, 0, 0, 1);
	sprite.render();

	sprite._translate = p;
	sprite._color = glm::vec4(1, 1, 1, 1);
	sprite.render();
	*/
}
