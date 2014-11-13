// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef StringWidget_H
#define StringWidget_H

#include "FontAdapter.h"
#include "WidgetShape.h"


class StringWidget : public Widget
{
	FontDescriptor _fontDescriptor;
	std::string _string;
	glm::mat4x4 _transform;
	glm::vec4 _color;
	glm::vec4 _glow;
	float _width;

public:
	StringWidget();
	StringWidget(const char* string, glm::vec2 translate);
	StringWidget(const char* string, glm::mat4x4 transform);

	const glm::mat4x4& GetTransform() const { return _transform; }
	void SetTransform(glm::mat4x4 value) { _transform = value; }
	void SetTranslate(glm::vec2 value) { _transform = glm::translate(glm::mat4(), glm::vec3(value, 0)); }

	const FontDescriptor& GetFontDescriptor() const;
	void SetFontDescriptor(const FontDescriptor& fontDescriptor);

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

private:
	virtual void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

	void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices, glm::vec4 color, float blur);

private:
	StringWidget(const StringWidget&) { }
	StringWidget& operator=(const StringWidget&) { return *this; }
};


#endif
