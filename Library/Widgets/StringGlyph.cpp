#include "StringGlyph.h"
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



StringGlyph::StringGlyph() :
	_string(),
	_alpha(1),
	_delta(0)
{
}


StringGlyph::StringGlyph(const char* string, glm::vec2 translate, float alpha, float delta) :
	_string(string),
	_alpha(alpha),
	_delta(delta)
{
	SetTranslate(translate);
}


StringGlyph::StringGlyph(const char* string, glm::mat4x4 transform, float alpha, float delta) :
	_string(string),
	_alpha(alpha),
	_delta(delta)
{
	SetTransform(transform);
}


const FontDescriptor& StringGlyph::GetFontDescriptor() const
{
	return _fontDescriptor;
}


void StringGlyph::SetFontDescriptor(const FontDescriptor& fontDescriptor)
{
	_fontDescriptor = fontDescriptor;
}


const char* StringGlyph::GetString() const
{
	return _string.c_str();
}


void StringGlyph::SetString(const char* value)
{
	_string = value;
}


const glm::vec3 StringGlyph::GetColor() const
{
	return _color;
}


void StringGlyph::SetColor(const glm::vec3& value)
{
	_color = value;
}


const float StringGlyph::GetAlpha() const
{
	return _alpha;
}


void StringGlyph::SetAlpha(float value)
{
	_alpha = value;
}


const float StringGlyph::GetDelta() const
{
	return _delta;
}


void StringGlyph::SetDelta(float value)
{
	_delta = value;
}


void StringGlyph::AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
	glm::vec2 p(0, 0);

	glm::vec4 colorize = glm::vec4(_color, 1);

	float alpha = _alpha;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv(".", L".");
	std::wstring ws = conv.from_bytes(_string);


	TextureFont* textureFont = GetWidgetShape()->GetTextureAtlas()->GetTextureFont(GetFontDescriptor());
	glm::mat4x4 transform = GetTransform();

	if (ContainsArabic(ws))
	{
		TextureChar* textureChar = textureFont->GetTextureChar(_string.c_str());

		bounds2f item_xy = textureChar->GetOuterXY(p);
		bounds2f item_uv = textureChar->GetOuterUV();
		float item_u0 = item_uv.min.x;
		float item_u1 = item_uv.max.x;
		float item_v0 = item_uv.min.y;
		float item_v1 = item_uv.max.y;

		glm::vec2 s = textureChar->GetInnerSize();

		bounds2f bounds;
		bounds.min = (transform * glm::vec4(item_xy.min.x, item_xy.min.y, 0, 1)).xy();
		bounds.max = (transform * glm::vec4(item_xy.max.x, item_xy.max.y, 0, 1)).xy();

		float next_alpha = alpha + _delta * s.x;

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

			TextureChar* textureChar = textureFont->GetTextureChar(character);

			bounds2f item_xy = textureChar->GetOuterXY(p);
			bounds2f item_uv = textureChar->GetOuterUV();
			float item_u0 = item_uv.min.x;
			float item_u1 = item_uv.max.x;
			float item_v0 = item_uv.min.y;
			float item_v1 = item_uv.max.y;

			glm::vec2 s = textureChar->GetInnerSize();

			bounds2f bounds;
			bounds.min = (transform * glm::vec4(item_xy.min.x, item_xy.min.y, 0, 1)).xy();
			bounds.max = (transform * glm::vec4(item_xy.max.x, item_xy.max.y, 0, 1)).xy();

			float next_alpha = alpha + _delta * s.x;

			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_00(), glm::vec2(item_u0, item_v1), colorize, alpha));
			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_01(), glm::vec2(item_u0, item_v0), colorize, alpha));
			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_11(), glm::vec2(item_u1, item_v0), colorize, next_alpha));
			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_11(), glm::vec2(item_u1, item_v0), colorize, next_alpha));
			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_10(), glm::vec2(item_u1, item_v1), colorize, next_alpha));
			vertices.push_back(Vertex_2f_2f_4f_1f(bounds.mix_00(), glm::vec2(item_u0, item_v1), colorize, alpha));

			if (next_alpha < 0)
				break;

			p.x += s.x;
			p.x = std::ceilf(p.x);

			alpha = next_alpha;
		}
	}
}
