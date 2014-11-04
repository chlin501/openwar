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
		WidgetShape* _shape;
	public:
		WidgetVertexBuffer(WidgetShape* shape);
		virtual void Update();
	};

	TextureAtlas* _textureAtlas;
	WidgetVertexBuffer _vertices;
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



class PatchShape
{
	friend class PatchGlyph;

	class PatchVertexBuffer : public VertexBuffer<Vertex_2f_2f_1f>
	{
		PatchShape* _shape;
	public:
		PatchVertexBuffer(PatchShape* shape);
		virtual void Update();
	};

	PatchVertexBuffer _vertices;
	std::vector<PatchGlyph*> _glyphs;

public:
	PatchShape();

	VertexBuffer<Vertex_2f_2f_1f>* GetVertices();

	void ClearGlyphs();
	void AddGlyph(PatchGlyph* glyph);
	void RemoveGlyph(PatchGlyph* glyph);

private:
	void UpdateVertexBuffer();
	void AppendShapeGlyph(std::vector<Vertex_2f_2f_1f>& vertices, PatchGlyph* shapeGlyph);
	void AppendRectangle(std::vector<Vertex_2f_2f_1f>& vertices, bounds2f xy, bounds2f uv);
};



class WidgetGlyph
{
public:
	virtual ~WidgetGlyph();
};


class PatchGlyph
{
	friend class PatchShape;
	PatchShape* _patchShape;

public:
	bounds2f outer_xy;
	bounds2f inner_xy;
	bounds2f outer_uv;
	bounds2f inner_uv;

	PatchGlyph();
	PatchGlyph(TextureImage* tile, bounds2f bounds, glm::vec2 inset);
	~PatchGlyph();

	void Reset();
	void Reset(TextureImage* tile, bounds2f bounds, glm::vec2 inset);

private:
	PatchGlyph(const PatchGlyph&) { }
	PatchGlyph& operator=(const PatchGlyph&) { return *this; }
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
	virtual ~StringGlyph();

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
