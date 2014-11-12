#ifndef StringGlyph_H
#define StringGlyph_H

#include "FontAdapter.h"
#include "WidgetShape.h"


class StringGlyph : public WidgetGlyph
{
	FontDescriptor _fontDescriptor;
	std::string _string;
	glm::vec4 _color;
	glm::vec4 _glow;
	float _width;

public:
	StringGlyph();
	StringGlyph(const char* string, glm::vec2 translate);
	StringGlyph(const char* string, glm::mat4x4 transform);

	const FontDescriptor& GetFontDescriptor() const;
	void SetFontDescriptor(const FontDescriptor& fontDescriptor);

	const char* GetString() const;
	void SetString(const char* value);

	const glm::vec4 GetColor() const;
	void SetColor(const glm::vec3& value);
	void SetColor(const glm::vec4& value);

	const glm::vec4 GetGlow() const;
	void SetGlow(const glm::vec4& value);

	const float GetAlpha() const;
	void SetAlpha(float value);

	const float GetWidth() const;
	void SetWidth(float value);

private:
	virtual void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

	void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices, glm::vec4 color, float blur);

private:
	StringGlyph(const StringGlyph&) { }
	StringGlyph& operator=(const StringGlyph&) { return *this; }
};


#endif
