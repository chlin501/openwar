// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef StringWidget_H
#define StringWidget_H

#include "Widget.h"
#include "FontAdapter.h"
#include "WidgetShape.h"

#include <glm/gtx/transform.hpp>


class StringWidget : public Widget
{
	FontDescriptor _fontDescriptor;
	std::string _string;
	glm::vec2 _position;
	glm::vec4 _color;
	glm::vec4 _glow;
	float _width;

public:
	StringWidget();
	StringWidget(const char* string, glm::vec2 translate);

	const FontDescriptor& GetFontDescriptor() const;
	void SetFontDescriptor(const FontDescriptor& fontDescriptor);

	glm::vec2 GetPosition() const;
	void SetPosition(glm::vec2 value);

	const char* GetString() const;
	void SetString(const char* value);

	const glm::vec4 GetColor() const;
	void SetColor(const glm::vec3& value);
	void SetColor(const glm::vec4& value);

	const glm::vec4 GetGlow() const;
	void SetGlow(const glm::vec4& value);

	const float GetAlpha() const;
	void SetAlpha(float value);

	const float GetWidth() const;
	void SetWidth(float value);

	virtual void AppendVertices(WidgetShape* widgetShape, std::vector<Vertex_2f_2f_4f_1f>& vertices);

private:
	void AppendVertices(WidgetShape* widgetShape, std::vector<Vertex_2f_2f_4f_1f>& vertices, glm::vec4 color, float blur);

private:
	StringWidget(const StringWidget&) { }
	StringWidget& operator=(const StringWidget&) { return *this; }
};


#endif
