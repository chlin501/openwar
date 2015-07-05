// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef StringWidget_H
#define StringWidget_H

#include "Widget.h"
#include "Graphics/FontAdapter.h"
#include "WidgetView.h"

#include <glm/gtx/transform.hpp>


enum class StringAlignment : int
{
	HorizontalLeft = 1,
	HorizontalCenter = 2,
	HorizontalRight = 4,
	VerticalBottom = 8,
	VerticalCenter = 16,
	VerticalTop = 32
};

enum class StringAnchor : int
{
	BottomLeft =   static_cast<int>(StringAlignment::HorizontalLeft)   | static_cast<int>(StringAlignment::VerticalBottom),
	BottomCenter = static_cast<int>(StringAlignment::HorizontalCenter) | static_cast<int>(StringAlignment::VerticalBottom),
	BottomRight =  static_cast<int>(StringAlignment::HorizontalRight)  | static_cast<int>(StringAlignment::VerticalBottom),
	CenterLeft =   static_cast<int>(StringAlignment::HorizontalLeft)   | static_cast<int>(StringAlignment::VerticalCenter),
	Center =       static_cast<int>(StringAlignment::HorizontalCenter) | static_cast<int>(StringAlignment::VerticalCenter),
	CenterRight =  static_cast<int>(StringAlignment::HorizontalRight)  | static_cast<int>(StringAlignment::VerticalCenter),
	TopLeft =      static_cast<int>(StringAlignment::HorizontalLeft)   | static_cast<int>(StringAlignment::VerticalTop),
	TopCenter =    static_cast<int>(StringAlignment::HorizontalCenter) | static_cast<int>(StringAlignment::VerticalTop),
	TopRight =     static_cast<int>(StringAlignment::HorizontalRight)  | static_cast<int>(StringAlignment::VerticalTop)
};


inline bool IsStringAnchorAlignment(StringAnchor a, StringAlignment f)
{
	return (static_cast<int>(a) & static_cast<int>(f)) != 0;
}



class StringWidget : public Widget
{
	glm::vec2 _position{};
	FontDescriptor _fontDescriptor{};
	std::string _original{};
	std::string _display{};
	glm::vec4 _color{1.0f, 1.0f, 1.0f, 1.0f};
	glm::vec4 _glowColor{};
	float _glowRadius{1};
	float _width{};
	StringAnchor _anchor{};

public:
	StringWidget(WidgetOwner* widgetOwner);

	virtual bounds2f GetBounds() const;

	virtual glm::vec2 GetPosition() const;
	virtual void SetPosition(glm::vec2 value);

	virtual StringAnchor GetAnchor() const;
	virtual void SetAnchor(StringAnchor value);

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
	void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices, glm::vec2 offset, glm::vec4 color, float blurRadius);
};


#endif
