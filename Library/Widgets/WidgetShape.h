#ifndef STRINGSHAPE_H
#define STRINGSHAPE_H

#include <map>
#include <glm/gtc/matrix_transform.hpp>


#include "Algebra/bounds.h"
#include "ShaderProgram.h"
#include "TextureAtlas.h"
#include "TextureFont.h"

class GraphicsContext;
class WidgetShape;
class StringGlyph;
class PatchGlyph;
class WidgetGlyph;


class WidgetShader : public ShaderProgram3<glm::vec2, glm::vec2, float>
{
	friend class GraphicsContext;
	/*
		attribute vec2 position;
		attribute vec2 texcoord;
		attribute float alpha;
		uniform mat4 transform;
		uniform sampler2D texture;
		uniform vec4 color;
	 */
	WidgetShader(GraphicsContext* gc);
};


class WidgetShape
{
	class WidgetVertexBuffer : public VertexBuffer<Vertex_2f_2f_1f>
	{
		WidgetShape* _widgetShape;
	public:
		WidgetVertexBuffer(WidgetShape* shape);
		virtual void Update();
	};

	TextureAtlas* _textureAtlas;
	WidgetVertexBuffer _vertices;
	std::vector<WidgetGlyph*> _widgetGlyphs;

public:
	explicit WidgetShape(TextureAtlas* textureAtlas);
	~WidgetShape();

	TextureAtlas* GetTextureAtlas() const;

	VertexBuffer<Vertex_2f_2f_1f>* GetVertices();

	void ClearGlyphs();
	void AddGlyph(WidgetGlyph* glyph);
	void RemoveGlyph(WidgetGlyph* glyph);

	glm::vec2 MeasureGlyph(StringGlyph* glyph) const;

private:
	void UpdateVertexBuffer();

	WidgetShape(const WidgetShape&) : _vertices(nullptr) { }
	WidgetShape& operator=(const WidgetShape&) { return *this; }
};


class WidgetGlyph
{
	friend class WidgetShape;

	WidgetShape* _widgetShape;
	glm::mat4x4 _transform;

public:
	WidgetGlyph();
	virtual ~WidgetGlyph();

	WidgetShape* GetWidgetShape() const;

	const glm::mat4x4& GetTransform() const { return _transform; }
	void SetTransform(glm::mat4x4 value) { _transform = value; }
	void SetTranslate(glm::vec2 value) { _transform = glm::translate(glm::mat4(), glm::vec3(value, 0)); }

private:
	virtual void AppendVertices(std::vector<Vertex_2f_2f_1f>& vertices) = 0;

private:
	WidgetGlyph(const WidgetGlyph&) { }
	WidgetGlyph& operator=(const WidgetGlyph&) { return *this; }
};


#include "PatchGlyph.h"
#include "StringGlyph.h"

#endif
