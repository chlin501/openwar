#include "WidgetShape.h"
#include "Image.h"
#include <glm/gtc/matrix_transform.hpp>

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



/***/


WidgetShape::WidgetVertexBuffer::WidgetVertexBuffer(WidgetShape* shape) :
	_widgetShape(shape)
{
}


void WidgetShape::WidgetVertexBuffer::Update()
{
	_widgetShape->UpdateVertexBuffer();
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
	for (WidgetGlyph* glyph : _widgetGlyphs)
		glyph->_widgetShape = nullptr;
}


TextureAtlas* WidgetShape::GetTextureAtlas() const
{
	return _textureAtlas;
}


VertexBuffer<Vertex_2f_2f_1f>* WidgetShape::GetVertices()
{
	return &_vertices;
}


void WidgetShape::ClearGlyphs()
{
	for (WidgetGlyph* glyph : _widgetGlyphs)
		glyph->_widgetShape = nullptr;
	_widgetGlyphs.clear();
}


void WidgetShape::AddGlyph(WidgetGlyph* glyph)
{
	if (glyph->_widgetShape != nullptr)
		glyph->_widgetShape->RemoveGlyph(glyph);

	glyph->_widgetShape = this;
	_widgetGlyphs.push_back(glyph);
}


void WidgetShape::RemoveGlyph(WidgetGlyph* glyph)
{
	glyph->_widgetShape = nullptr;
	_widgetGlyphs.erase(
		std::remove(_widgetGlyphs.begin(), _widgetGlyphs.end(), glyph),
		_widgetGlyphs.end());
}


glm::vec2 WidgetShape::MeasureGlyph(StringGlyph* glyph) const
{
	TextureFont* textureFont = _textureAtlas->GetTextureFont(glyph->GetFontDescriptor());

	return textureFont->MeasureText(glyph->GetString());
}


void WidgetShape::UpdateVertexBuffer()
{
	static std::vector<Vertex_2f_2f_1f> vertices;

	for (WidgetGlyph* glyph : _widgetGlyphs)
		glyph->AppendVertices(vertices);

	_vertices.UpdateVBO(GL_TRIANGLES, vertices.data(), vertices.size());
	vertices.clear();
}




/***/


WidgetGlyph::WidgetGlyph()
	: _widgetShape(nullptr)
{
}


WidgetGlyph::~WidgetGlyph()
{
	if (_widgetShape != nullptr)
		_widgetShape->RemoveGlyph(this);
}


WidgetShape* WidgetGlyph::GetWidgetShape() const
{
	return _widgetShape;
}


/***/




/***/

