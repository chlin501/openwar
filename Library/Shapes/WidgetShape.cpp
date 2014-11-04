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
	_blend_sfactor = GL_ONE;
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


WidgetShape::WidgetVertexBuffer::WidgetVertexBuffer(WidgetShape* shape) :
	_shape(shape)
{
}


void WidgetShape::WidgetVertexBuffer::Update()
{
	_shape->UpdateVertexBuffer();
}


/***/



WidgetShape::WidgetShape(TextureAtlas* textureAtlas) :
	_vertices(this),
	_textureAtlas(textureAtlas)
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


glm::vec2 WidgetShape::MeasureGlyph(StringGlyph* glyph) const
{
	TextureFont* textureFont = _textureAtlas->GetTextureFont(glyph->GetFontDescriptor());

	return textureFont->MeasureText(glyph->GetString());
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

	TextureFont* textureFont = _textureAtlas->GetTextureFont(glyph->GetFontDescriptor());

	if (ContainsArabic(ws))
	{
		TextureChar* textureChar = textureFont->GetTextureChar(glyph->_string.c_str());

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

			TextureChar* textureChar = textureFont->GetTextureChar(character);

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



PatchShape::PatchVertexBuffer::PatchVertexBuffer(PatchShape* shape) :
	_shape(shape)
{
	_mode = GL_TRIANGLES;
}


void PatchShape::PatchVertexBuffer::Update()
{
	_shape->UpdateVertexBuffer();
}


PatchShape::PatchShape() :
	_vertices(this)
{
}


VertexBuffer<Vertex_2f_2f_1f>* PatchShape::GetVertices()
{
	return &_vertices;
}


void PatchShape::ClearGlyphs()
{
	for (PatchGlyph* glyph : _glyphs)
		glyph->_patchShape = nullptr;
	_glyphs.clear();
}


void PatchShape::AddGlyph(PatchGlyph* glyph)
{
	if (glyph->_patchShape != nullptr)
		glyph->_patchShape->RemoveGlyph(glyph);
	glyph->_patchShape = this;
	_glyphs.push_back(glyph);
}


void PatchShape::RemoveGlyph(PatchGlyph* glyph)
{
	glyph->_patchShape = nullptr;
	_glyphs.erase(
		std::find(_glyphs.begin(), _glyphs.end(), glyph),
		_glyphs.end());
}


void PatchShape::UpdateVertexBuffer()
{
	static std::vector<Vertex_2f_2f_1f> vertices;

	for (PatchGlyph* glyph : _glyphs)
		AppendShapeGlyph(vertices, glyph);

	_vertices.UpdateVBO(GL_TRIANGLES, vertices.data(), vertices.size());
	vertices.clear();
}


void PatchShape::AppendShapeGlyph(std::vector<Vertex_2f_2f_1f>& vertices, PatchGlyph* patchGlyph)
{
	bounds2f ixy = patchGlyph->inner_xy;
	bounds2f oxy = patchGlyph->outer_xy;
	bounds2f iuv = patchGlyph->inner_uv;
	bounds2f ouv = patchGlyph->outer_uv;

	bool min_x = oxy.min.x < ixy.min.x;
	bool max_x = ixy.max.x < oxy.max.x;
	bool min_y = oxy.min.y < ixy.min.y;
	bool max_y = ixy.max.y < oxy.max.y;

	if (min_x && min_y)
		AppendRectangle(vertices,
			bounds2f(oxy.min.x, oxy.min.y, ixy.min.x, ixy.min.y),
			bounds2f(ouv.min.x, ouv.max.y, iuv.min.x, iuv.max.y));

	if (min_x)
		AppendRectangle(vertices,
			bounds2f(oxy.min.x, ixy.min.y, ixy.min.x, ixy.max.y),
			bounds2f(ouv.min.x, iuv.max.y, iuv.min.x, iuv.min.y));

	if (min_x && max_y)
		AppendRectangle(vertices,
			bounds2f(oxy.min.x, ixy.max.y, ixy.min.x, oxy.max.y),
			bounds2f(ouv.min.x, iuv.min.y, iuv.min.x, ouv.min.y));

	if (min_y)
		AppendRectangle(vertices,
			bounds2f(ixy.min.x, oxy.min.y, ixy.max.x, ixy.min.y),
			bounds2f(iuv.min.x, ouv.max.y, iuv.max.x, iuv.max.y));

	AppendRectangle(vertices,
		bounds2f(ixy.min.x, ixy.min.y, ixy.max.x, ixy.max.y),
		bounds2f(iuv.min.x, iuv.max.y, iuv.max.x, iuv.min.y));

	if (max_y)
		AppendRectangle(vertices,
			bounds2f(ixy.min.x, ixy.max.y, ixy.max.x, oxy.max.y),
			bounds2f(iuv.min.x, iuv.min.y, iuv.max.x, ouv.min.y));

	if (max_x && min_y)
		AppendRectangle(vertices,
			bounds2f(ixy.max.x, oxy.min.y, oxy.max.x, ixy.min.y),
			bounds2f(iuv.max.x, ouv.max.y, ouv.max.x, iuv.max.y));

	if (max_x)
		AppendRectangle(vertices,
			bounds2f(ixy.max.x, ixy.min.y, oxy.max.x, ixy.max.y),
			bounds2f(iuv.max.x, iuv.max.y, ouv.max.x, iuv.min.y));

	if (max_x && max_y)
		AppendRectangle(vertices,
			bounds2f(ixy.max.x, ixy.max.y, oxy.max.x, oxy.max.y),
			bounds2f(iuv.max.x, iuv.min.y, ouv.max.x, ouv.min.y));
}


void PatchShape::AppendRectangle(std::vector<Vertex_2f_2f_1f>& vertices, bounds2f xy, bounds2f uv)
{
	vertices.push_back(Vertex_2f_2f_1f(glm::vec2(xy.min.x, xy.min.y), glm::vec2(uv.min.x, uv.min.y), 1.0f));
	vertices.push_back(Vertex_2f_2f_1f(glm::vec2(xy.min.x, xy.max.y), glm::vec2(uv.min.x, uv.max.y), 1.0f));
	vertices.push_back(Vertex_2f_2f_1f(glm::vec2(xy.max.x, xy.max.y), glm::vec2(uv.max.x, uv.max.y), 1.0f));
	vertices.push_back(Vertex_2f_2f_1f(glm::vec2(xy.max.x, xy.max.y), glm::vec2(uv.max.x, uv.max.y), 1.0f));
	vertices.push_back(Vertex_2f_2f_1f(glm::vec2(xy.max.x, xy.min.y), glm::vec2(uv.max.x, uv.min.y), 1.0f));
	vertices.push_back(Vertex_2f_2f_1f(glm::vec2(xy.min.x, xy.min.y), glm::vec2(uv.min.x, uv.min.y), 1.0f));
}



/***/


WidgetGlyph::~WidgetGlyph()
{

}


/***/


PatchGlyph::PatchGlyph() :
	_patchShape(nullptr)
{

}


PatchGlyph::PatchGlyph(TextureImage* tile, bounds2f bounds, glm::vec2 inset) :
	_patchShape(nullptr)
{
	outer_xy = bounds;
	inner_xy = bounds.grow(-inset.x, -inset.y);
	outer_uv = tile->GetOuterUV();
	inner_uv = tile->GetInnerUV();
}


PatchGlyph::~PatchGlyph()
{
	if (_patchShape != nullptr)
		_patchShape->RemoveGlyph(this);
}


void PatchGlyph::Reset()
{
	outer_xy = bounds2f();
	inner_xy = bounds2f();
	outer_uv = bounds2f();
	inner_uv = bounds2f();
}


void PatchGlyph::Reset(TextureImage* tile, bounds2f bounds, glm::vec2 inset)
{
	outer_xy = bounds;
	inner_xy = bounds.grow(-inset.x, -inset.y);
	outer_uv = tile->GetOuterUV();
	inner_uv = tile->GetInnerUV();
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


const FontDescriptor& StringGlyph::GetFontDescriptor() const
{
	return _fontDescriptor;
}


void StringGlyph::SetFontDescriptor(const FontDescriptor& fontDescriptor)
{
	_fontDescriptor = fontDescriptor;
}
