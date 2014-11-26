// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef StringWidget_H
#define StringWidget_H

#include "Widget.h"
#include "Graphics/FontAdapter.h"
#include "WidgetView.h"

#include <glm/gtx/transform.hpp>


class StringWidget : public Widget
{
	glm::vec2 _position;
	FontDescriptor _fontDescriptor;
	std::string _string;
	glm::vec4 _color;
	glm::vec4 _glowColor;
	float _glowRadius;
	float _width;

public:
	StringWidget(WidgetOwner* widgetOwner);

	virtual glm::vec2 GetPosition() const;
	virtual void SetPosition(glm::vec2 value);

	virtual glm::vec2 MeasureSize() const;

	virtual const float GetWidth() const;
	virtual void SetWidth(float value);

	virtual const FontDescriptor& GetFontDescriptor() const;
	virtual void SetFontDescriptor(const FontDescriptor& fontDescriptor);

	virtual const char* GetString() const;
	virtual void SetString(const char* value);

	virtual const glm::vec4 GetColor() const;
	virtual void SetColor(const glm::vec3& value);
	virtual void SetColor(const glm::vec4& value);

	virtual const glm::vec4 GetGlowColor() const;
	virtual void SetGlowColor(const glm::vec4& value);

	virtual const float GetGlowRadius() const;
	virtual void SetGlowRadius(float value);

	virtual const float GetAlpha() const;
	virtual void SetAlpha(float value);

	virtual void OnTouchEnter(Touch* touch);
	virtual void OnTouchBegin(Touch* touch);

	virtual void RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

private:
	void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices, glm::vec4 color, float blurRadius);

private:
	StringWidget(const StringWidget&) : Widget(nullptr) { }
	StringWidget& operator=(const StringWidget&) { return *this; }
};


#endif
