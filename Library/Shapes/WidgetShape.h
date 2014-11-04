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
	class StringVertexBuffer : public VertexBuffer<Vertex_2f_2f_1f>
	{
		WidgetShape* _shape;
	public:
		StringVertexBuffer(WidgetShape* shape);
		virtual void Update();
	};

	TextureAtlas* _textureAtlas;
	StringVertexBuffer _vertices;
	std::vector<StringGlyph*> _glyphs;

public:
	explicit WidgetShape(TextureAtlas* textureAtlas);
	~WidgetShape();

	VertexBuffer<Vertex_2f_2f_1f>* GetVertices();

	void ClearGlyphs();
	void AddGlyph(StringGlyph* glyph);
	void RemoveGlyph(StringGlyph* glyph);

	glm::vec2 MeasureGlyph(StringGlyph* glyph) const;

private:
	void UpdateVertexBuffer();
	void AppendStringGlyph(std::vector<Vertex_2f_2f_1f>& vertices, StringGlyph* glyph);

	WidgetShape(const WidgetShape&) : _vertices(nullptr) { }
	WidgetShape& operator=(const WidgetShape&) { return *this; }
};


class StringGlyph
{
	friend class WidgetShape;

	WidgetShape* _shape;
	FontDescriptor _fontDescriptor;
	std::string _string;
	glm::mat4x4 _transform;
	float _alpha;
	float _delta;

public:
	StringGlyph();
	StringGlyph(const char* string, glm::vec2 translate, float alpha = 1, float delta = 0);
	StringGlyph(const char* string, glm::mat4x4 transform, float alpha = 1, float delta = 0);
	~StringGlyph();

	const FontDescriptor& GetFontDescriptor() const;
	void SetFontDescriptor(const FontDescriptor& fontDescriptor);

	const char* GetString() const { return _string.c_str(); }
	void SetString(const char* value) { _string = value; }

	const glm::mat4x4 GetTransform() const { return _transform; }
	void SetTransform(glm::mat4x4 value) { _transform = value; }
	void SetTranslate(glm::vec2 value) { _transform = glm::translate(glm::mat4(), glm::vec3(value, 0)); }

	const float GetAlpha() const { return _alpha; }
	void SetAlpha(float value) { _alpha = value; }

	const float GetDelta() const { return _delta; }
	void SetDelta(float value) { _delta = value; }

private:
	StringGlyph(const StringGlyph&) { }
	StringGlyph& operator=(const StringGlyph&) { return *this; }
};


#endif
