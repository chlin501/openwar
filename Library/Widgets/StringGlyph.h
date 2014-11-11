#ifndef StringGlyph_H
#define StringGlyph_H

#include "WidgetShape.h"


class StringGlyph : public WidgetGlyph
{
	FontDescriptor _fontDescriptor;
	std::string _string;
	glm::vec3 _color;
	float _alpha;
	float _delta;

public:
	StringGlyph();
	StringGlyph(const char* string, glm::vec2 translate, float alpha = 1, float delta = 0);
	StringGlyph(const char* string, glm::mat4x4 transform, float alpha = 1, float delta = 0);

	const FontDescriptor& GetFontDescriptor() const;
	void SetFontDescriptor(const FontDescriptor& fontDescriptor);

	const char* GetString() const;
	void SetString(const char* value);

	const glm::vec3 GetColor() const;
	void SetColor(const glm::vec3& value);

	const float GetAlpha() const;
	void SetAlpha(float value);

	const float GetDelta() const;
	void SetDelta(float value);

private:
	virtual void AppendVertices(std::vector<Vertex_2f_2f_1f>& vertices);

private:
	StringGlyph(const StringGlyph&) { }
	StringGlyph& operator=(const StringGlyph&) { return *this; }
};


#endif
