// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "StringWidget.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/TextureFont.h"
#include "Surface/Touch.h"

#include <codecvt>
#include <cstdlib>
#include <locale>


static bool IsArabic(wchar_t wc)
{
	if (0x0600 <= wc && wc <= 0x08FF)
		return true;

	if (0xFB00 <= wc && wc <= 0xFDFF)
		return true;

	return false;
}


static bool ContainsArabic(const std::wstring& ws)
{
	for (wchar_t wc : ws)
		if (IsArabic(wc))
			return true;

	return false;
}



StringWidget::StringWidget(WidgetOwner* widgetOwner) : Widget(widgetOwner),
	_string(),
	_color(1, 1, 1, 1),
	_width(0)
{
}


glm::vec2 StringWidget::GetPosition() const
{
	return _position;
}


void StringWidget::SetPosition(glm::vec2 value)
{
	_position = value;
}


glm::vec2 StringWidget::MeasureSize() const
{
	TextureFont* textureFont = GetWidgetView()->GetTextureAtlas()->GetTextureFont(_fontDescriptor);

	return textureFont->MeasureText(_string.c_str());
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
	return _string.c_str();
}


void StringWidget::SetString(const char* value)
{
	_string = value;
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


const glm::vec4 StringWidget::GetGlow() const
{
	return _glow;
}


void StringWidget::SetGlow(const glm::vec4& value)
{
	_glow = value;
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
	if (_glow.a != 0)
		AppendVertices(vertices, _glow, 2);

	AppendVertices(vertices, _color, 0);
}


void StringWidget::AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices, glm::vec4 color, float blur)
{
	glm::vec2 offset = _position;
	glm::vec2 p(0, 0);

	float alpha = color.a;
	float delta = 0;

	if (_width != 0)
	{
		float fade = 8.0f;
		alpha = _width / fade - 0.5f;
		delta = -1.0f / fade;
	}

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv(".", L".");
	std::wstring ws = conv.from_bytes(_string);

	TextureFont* textureFont = GetWidgetView()->GetTextureAtlas()->GetTextureFont(GetFontDescriptor());

	if (ContainsArabic(ws))
	{
		TextureChar* textureChar = textureFont->GetTextureChar(_string.c_str(), blur);
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

		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_00(), glm::vec2(item_u0, item_v1), colorize, alpha));
		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_01(), glm::vec2(item_u0, item_v0), colorize, alpha));
		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_11(), glm::vec2(item_u1, item_v0), colorize, next_alpha));
		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_11(), glm::vec2(item_u1, item_v0), colorize, next_alpha));
		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_10(), glm::vec2(item_u1, item_v1), colorize, next_alpha));
		vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_00(), glm::vec2(item_u0, item_v1), colorize, alpha));
	}
	else
	{
		for (wchar_t wc : ws)
		{
			if (wc == 0)
				continue;

			std::string character = conv.to_bytes(&wc, &wc + 1);
			if (character.empty())
				continue;

			TextureChar* textureChar = textureFont->GetTextureChar(character, blur);
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

			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_00(), glm::vec2(item_u0, item_v1), colorize, alpha));
			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_01(), glm::vec2(item_u0, item_v0), colorize, alpha));
			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_11(), glm::vec2(item_u1, item_v0), colorize, next_alpha));
			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_11(), glm::vec2(item_u1, item_v0), colorize, next_alpha));
			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_10(), glm::vec2(item_u1, item_v1), colorize, next_alpha));
			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_00(), glm::vec2(item_u0, item_v1), colorize, alpha));

			if (next_alpha < 0)
				break;

			p.x += s.x;
			//p.x = std::ceilf(p.x);

			alpha = next_alpha;
		}
	}
}
