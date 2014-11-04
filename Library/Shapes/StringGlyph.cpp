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
	_transform(),
	_alpha(1),
	_delta(0)
{
}


StringGlyph::StringGlyph(const char* string, glm::vec2 translate, float alpha, float delta) :
	_string(string),
	_transform(glm::translate(glm::mat4(), glm::vec3(translate, 0))),
	_alpha(alpha),
	_delta(delta)
{
}


StringGlyph::StringGlyph(const char* string, glm::mat4x4 transform, float alpha, float delta) :
	_string(string),
	_transform(transform),
	_alpha(alpha),
	_delta(delta)
{
}


const FontDescriptor& StringGlyph::GetFontDescriptor() const
{
	return _fontDescriptor;
}


void StringGlyph::SetFontDescriptor(const FontDescriptor& fontDescriptor)
{
	_fontDescriptor = fontDescriptor;
}


void StringGlyph::AppendVertices(std::vector<Vertex_2f_2f_1f>& vertices)
{
	glm::vec2 p(0, 0);
	float alpha = _alpha;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv(".", L".");
	std::wstring ws = conv.from_bytes(_string);


	TextureFont* textureFont = GetWidgetShape()->GetTextureAtlas()->GetTextureFont(GetFontDescriptor());

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
		bounds.min = (_transform * glm::vec4(item_xy.min.x, item_xy.min.y, 0, 1)).xy();
		bounds.max = (_transform * glm::vec4(item_xy.max.x, item_xy.max.y, 0, 1)).xy();

		float next_alpha = alpha + _delta * s.x;

		vertices.push_back(Vertex_2f_2f_1f(bounds.p11(), glm::vec2(item_u0, item_v1), alpha));
		vertices.push_back(Vertex_2f_2f_1f(bounds.p12(), glm::vec2(item_u0, item_v0), alpha));
		vertices.push_back(Vertex_2f_2f_1f(bounds.p22(), glm::vec2(item_u1, item_v0), next_alpha));
		vertices.push_back(Vertex_2f_2f_1f(bounds.p22(), glm::vec2(item_u1, item_v0), next_alpha));
		vertices.push_back(Vertex_2f_2f_1f(bounds.p21(), glm::vec2(item_u1, item_v1), next_alpha));
		vertices.push_back(Vertex_2f_2f_1f(bounds.p11(), glm::vec2(item_u0, item_v1), alpha));
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
			bounds.min = (_transform * glm::vec4(item_xy.min.x, item_xy.min.y, 0, 1)).xy();
			bounds.max = (_transform * glm::vec4(item_xy.max.x, item_xy.max.y, 0, 1)).xy();

			float next_alpha = alpha + _delta * s.x;

			vertices.push_back(Vertex_2f_2f_1f(bounds.p11(), glm::vec2(item_u0, item_v1), alpha));
			vertices.push_back(Vertex_2f_2f_1f(bounds.p12(), glm::vec2(item_u0, item_v0), alpha));
			vertices.push_back(Vertex_2f_2f_1f(bounds.p22(), glm::vec2(item_u1, item_v0), next_alpha));
			vertices.push_back(Vertex_2f_2f_1f(bounds.p22(), glm::vec2(item_u1, item_v0), next_alpha));
			vertices.push_back(Vertex_2f_2f_1f(bounds.p21(), glm::vec2(item_u1, item_v1), next_alpha));
			vertices.push_back(Vertex_2f_2f_1f(bounds.p11(), glm::vec2(item_u0, item_v1), alpha));

			if (next_alpha < 0)
				break;

			p.x += s.x;
			p.x = std::ceilf(p.x);

			alpha = next_alpha;
		}
	}
}
