#include "WidgetShape.h"
#include "Image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <codecvt>
#include <cstdlib>
#include <locale>

#include "TextureFont.h"
#include "GraphicsContext.h"


WidgetShader::WidgetShader(GraphicsContext* gc) : ShaderProgram3<glm::vec2, glm::vec2, float>(
	"position", "texcoord", "alpha",
	VERTEX_SHADER
	({
		attribute vec2 position;
		attribute vec2 texcoord;
		attribute float alpha;
		uniform mat4 transform;
		varying vec2 _texcoord;
		varying float _alpha;

		void main()
		{
			vec4 p = transform * vec4(position.x, position.y, 0, 1);

			_texcoord = texcoord;
			_alpha = alpha;

			gl_Position = vec4(p.x, p.y, 0.5, 1.0);
		}
	}),
	FRAGMENT_SHADER
	({
		uniform sampler2D texture;
		uniform vec4 color;
		varying vec2 _texcoord;
		varying float _alpha;

		void main()
		{
			vec4 result;
			//result.rgb = color.rgb;
			//result.a = texture2D(texture, _texcoord).a * color.a * clamp(_alpha, 0.0, 1.0);
			result = texture2D(texture, _texcoord);

			gl_FragColor = result;
		}
	}))
{
	_blend_sfactor = GL_SRC_ALPHA;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


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


/***/


WidgetShape::StringVertexBuffer::StringVertexBuffer(WidgetShape* shape) :
	_shape(shape)
{
}


void WidgetShape::StringVertexBuffer::Update()
{
	_shape->UpdateVertexBuffer();
}


/***/



WidgetShape::WidgetShape(TextureFont* font) :
	_vertices(this),
	_font(font)
{
	_vertices._mode = GL_TRIANGLES;
}


WidgetShape::~WidgetShape()
{
	for (StringGlyph* glyph : _glyphs)
		glyph->_shape = nullptr;
}


VertexBuffer<Vertex_2f_2f_1f>* WidgetShape::GetVertices()
{
	return &_vertices;
}


void WidgetShape::ClearGlyphs()
{
	for (StringGlyph* glyph : _glyphs)
		glyph->_shape = nullptr;
	_glyphs.clear();
}


void WidgetShape::AddGlyph(StringGlyph* glyph)
{
	if (glyph->_shape != nullptr)
		glyph->_shape->RemoveGlyph(glyph);

	glyph->_shape = this;
	_glyphs.push_back(glyph);
}


void WidgetShape::RemoveGlyph(StringGlyph* glyph)
{
	glyph->_shape = nullptr;
	_glyphs.erase(
		std::remove(_glyphs.begin(), _glyphs.end(), glyph),
		_glyphs.end());
}


void WidgetShape::UpdateVertexBuffer()
{
	static std::vector<Vertex_2f_2f_1f> vertices;

	for (StringGlyph* glyph : _glyphs)
		AppendStringGlyph(vertices, glyph);

	_vertices.UpdateVBO(GL_TRIANGLES, vertices.data(), vertices.size());
	vertices.clear();
}


void WidgetShape::AppendStringGlyph(std::vector<Vertex_2f_2f_1f>& vertices, StringGlyph* glyph)
{
	glm::vec2 p(0, 0);
	float alpha = glyph->_alpha;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv(".", L".");
	std::wstring ws = conv.from_bytes(glyph->_string);

	if (ContainsArabic(ws))
	{
		TextureChar* textureChar = _font->GetTextureChar(glyph->_string.c_str());

		bounds2f item_xy = textureChar->GetOuterXY(p);
		bounds2f item_uv = textureChar->GetOuterUV();
		float item_u0 = item_uv.min.x;
		float item_u1 = item_uv.max.x;
		float item_v0 = item_uv.min.y;
		float item_v1 = item_uv.max.y;

		glm::vec2 s = textureChar->GetInnerSize();

		bounds2f bounds;
		bounds.min = (glyph->_transform * glm::vec4(item_xy.min.x, item_xy.min.y, 0, 1)).xy();
		bounds.max = (glyph->_transform * glm::vec4(item_xy.max.x, item_xy.max.y, 0, 1)).xy();

		float next_alpha = alpha + glyph->_delta * s.x;

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

			TextureChar* textureChar = _font->GetTextureChar(character);


			bounds2f item_xy = textureChar->GetOuterXY(p);
			bounds2f item_uv = textureChar->GetOuterUV();
			float item_u0 = item_uv.min.x;
			float item_u1 = item_uv.max.x;
			float item_v0 = item_uv.min.y;
			float item_v1 = item_uv.max.y;

			glm::vec2 s = textureChar->GetInnerSize();

			bounds2f bounds;
			bounds.min = (glyph->_transform * glm::vec4(item_xy.min.x, item_xy.min.y, 0, 1)).xy();
			bounds.max = (glyph->_transform * glm::vec4(item_xy.max.x, item_xy.max.y, 0, 1)).xy();

			float next_alpha = alpha + glyph->_delta * s.x;

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


/***/



StringGlyph::StringGlyph() :
	_shape(nullptr),
	_string(),
	_transform(),
	_alpha(1),
	_delta(0)
{
}


StringGlyph::StringGlyph(const char* string, glm::vec2 translate, float alpha, float delta) :
	_shape(nullptr),
	_string(string),
	_transform(glm::translate(glm::mat4(), glm::vec3(translate, 0))),
	_alpha(alpha),
	_delta(delta)
{
}


StringGlyph::StringGlyph(const char* string, glm::mat4x4 transform, float alpha, float delta) :
	_shape(nullptr),
	_string(string),
	_transform(transform),
	_alpha(alpha),
	_delta(delta)
{
}


StringGlyph::~StringGlyph()
{
	if (_shape != nullptr)
		_shape->RemoveGlyph(this);
}


