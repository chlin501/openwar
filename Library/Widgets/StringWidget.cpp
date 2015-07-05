// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "StringWidget.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/TextureFont.h"
#include "Surface/Touch.h"

#include <codecvt>
#include <cstdlib>
#include <locale>


#ifndef OPENWAR_USE_UBIDI
#error "OPENWAR_USE_UBIDI is undefined"
#endif

#if OPENWAR_USE_UBIDI

#import <Foundation/Foundation.h>
#include "unicode/ubidi.h"


static UChar* ReserveSrcBuffer(std::size_t required)
{
	static UChar* buffer = nullptr;
	static std::size_t reserved = 0;

	if (buffer == nullptr || required > reserved)
	{
		delete buffer;
		buffer = new UChar[required];
		reserved = required;
	}

	return buffer;
}


static UChar* ReserveDstBuffer(std::size_t required)
{
	static UChar* buffer = nullptr;
	static std::size_t reserved = 0;

	if (buffer == nullptr || required > reserved)
	{
		delete buffer;
		buffer = new UChar[required];
		reserved = required;
	}

	return buffer;
}


static std::string ReorderToDisplayDirection(const std::string& s)
{
	if (TextureCharIterator::ContainsArabic(s.c_str()))
		return s;

	UErrorCode error = U_ZERO_ERROR;

	UChar* src = ReserveSrcBuffer(s.size());
	UChar* dst = ReserveDstBuffer(s.size() * 2);

	bool reorder = false;
	int length = 0;
	const char* p = s.c_str();
	while (*p != '\0')
	{
		std::size_t n = TextureCharIterator::Utf8CharSize(p);
		src[length] = static_cast<UChar>(TextureCharIterator::Utf8CharCode(p, n));

		if (ubidi_getBaseDirection(src + length,  1) != UBIDI_LTR)
			reorder = true;

		p += n;
		++length;
	}

	if (!reorder)
		return s;

	UBiDi* ubidi = ubidi_openSized(length, 0, &error);
	if (error != 0)
		NSLog(@"ubidi_openSized error:%04x", error);

	ubidi_setPara(ubidi, src, length, UBIDI_DEFAULT_LTR, NULL, &error);
	if (error != 0)
		NSLog(@"ubidi_setPara error:%04x", error);

	length = ubidi_writeReordered(ubidi, dst, length * 2, UBIDI_DO_MIRRORING | UBIDI_REMOVE_BIDI_CONTROLS, &error);
	if (error != 0)
		NSLog(@"ubidi_writeReordered error:%04x", error);

	NSString* result = [NSString stringWithCharacters:dst length:(NSUInteger)length];

	ubidi_close(ubidi);

	return std::string{result.UTF8String};
}


#else


static std::string ReorderToDisplayDirection(const std::string& s)
{
	return s;
}


#endif



StringWidget::StringWidget(WidgetOwner* widgetOwner) : Widget(widgetOwner)
{
}


bounds2f StringWidget::GetBounds() const
{
	glm::vec2 p = _position;
	glm::vec2 s = MeasureSize();

	if (_anchor != StringAnchor::BottomLeft)
	{
		if (IsStringAnchorAlignment(_anchor, StringAlignment::HorizontalCenter))
			p.x -= s.x / 2.0;
		else if (IsStringAnchorAlignment(_anchor, StringAlignment::HorizontalRight))
			p.x -= s.x;

		if (IsStringAnchorAlignment(_anchor, StringAlignment::VerticalCenter))
			p.y -= s.y / 2.0;
		else if (IsStringAnchorAlignment(_anchor, StringAlignment::VerticalTop))
			p.y -= s.y;
	}

	if (_width > 0)
		s.x = glm::min(s.x, _width);

	return bounds2f{p, p + s};
}


glm::vec2 StringWidget::GetPosition() const
{
	return _position;
}


void StringWidget::SetPosition(glm::vec2 value)
{
	_position = value;
}


StringAnchor StringWidget::GetAnchor() const
{
	return _anchor;
}


void StringWidget::SetAnchor(StringAnchor value)
{
	_anchor = value;
}


glm::vec2 StringWidget::MeasureSize() const
{
	TextureFont* textureFont = GetWidgetView()->GetWidgetTextureAtlas()->GetTextureFont(_fontDescriptor);

	return textureFont->MeasureText(_display.c_str());
}


const float StringWidget::GetWidth() const
{
	return _width;
}


void StringWidget::SetWidth(float value)
{
	_width = value;
}


const FontDescriptor& StringWidget::GetFontDescriptor() const
{
	return _fontDescriptor;
}


void StringWidget::SetFontDescriptor(const FontDescriptor& fontDescriptor)
{
	_fontDescriptor = fontDescriptor;
}


const char* StringWidget::GetString() const
{
	return _original.c_str();
}


void StringWidget::SetString(const char* value)
{
	_original = value;
	_display = ReorderToDisplayDirection(_original);
}


const glm::vec4 StringWidget::GetColor() const
{
	return _color;
}


void StringWidget::SetColor(const glm::vec3& value)
{
	_color = glm::vec4(value, _color.a);
}


void StringWidget::SetColor(const glm::vec4& value)
{
	_color = value;
}


const glm::vec4 StringWidget::GetGlowColor() const
{
	return _glowColor;
}


void StringWidget::SetGlowColor(const glm::vec4& value)
{
	_glowColor = value;
}


const float StringWidget::GetGlowRadius() const
{
	return _glowRadius;
}


void StringWidget::SetGlowRadius(float value)
{
	_glowRadius = value;
}


const float StringWidget::GetAlpha() const
{
	return _color.a;
}


void StringWidget::SetAlpha(float value)
{
	_color.a = value;
}


void StringWidget::OnTouchEnter(Touch* touch)
{
}


void StringWidget::OnTouchBegin(Touch* touch)
{
}


void StringWidget::RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
	glm::vec2 offset = GetBounds().min;

	if (_glowColor.a != 0 && _glowRadius > 0)
	{
		AppendVertices(vertices, offset, _glowColor, _glowRadius);
	}

	AppendVertices(vertices, offset, _color, 0);
}


void StringWidget::AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices, glm::vec2 offset, glm::vec4 color, float blurRadius)
{
	glm::vec2 p(0, 0);

	float alpha = color.a;
	float delta = 0;

	if (_width != 0)
	{
		float fade = 8.0f;
		alpha = _width / fade - 0.5f;
		delta = -1.0f / fade;
	}

	TextureFont* textureFont = GetWidgetView()->GetWidgetTextureAtlas()->GetTextureFont(GetFontDescriptor());

	TextureCharIterator chars(_display.c_str());
	while (!chars.GetChar().empty())
	{
		TextureChar* textureChar = textureFont->GetTextureChar(chars.GetChar(), blurRadius);
		glm::vec4 colorize = textureChar->CanColorize() ? glm::vec4(color.rgb(), 1) : glm::vec4();

		bounds2f item_xy = textureChar->GetOuterXY(p);
		bounds2f item_uv = textureChar->GetOuterUV();
		float item_u0 = item_uv.min.x;
		float item_u1 = item_uv.max.x;
		float item_v0 = item_uv.min.y;
		float item_v1 = item_uv.max.y;

		glm::vec2 s = textureChar->GetInnerSize();

		bounds2f bounds = item_xy + offset;

		float next_alpha = alpha + delta * s.x;

		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.fix<0,0>(), glm::vec2(item_u0, item_v1), colorize, alpha));
		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.fix<0,1>(), glm::vec2(item_u0, item_v0), colorize, alpha));
		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.fix<1,1>(), glm::vec2(item_u1, item_v0), colorize, next_alpha));
		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.fix<1,1>(), glm::vec2(item_u1, item_v0), colorize, next_alpha));
		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.fix<1,0>(), glm::vec2(item_u1, item_v1), colorize, next_alpha));
		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.fix<0,0>(), glm::vec2(item_u0, item_v1), colorize, alpha));

		if (next_alpha < 0)
			break;

		p.x += s.x;

		alpha = next_alpha;

		chars.MoveNext();
	}
}
