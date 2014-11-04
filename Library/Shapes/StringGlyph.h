#ifndef StringGlyph_H
#define StringGlyph_H

#include "WidgetShape.h"


class StringGlyph : public WidgetGlyph
{
	FontDescriptor _fontDescriptor;
	std::string _string;
	float _alpha;
	float _delta;

public:
	StringGlyph();
	StringGlyph(const char* string, glm::vec2 translate, float alpha = 1, float delta = 0);
	StringGlyph(const char* string, glm::mat4x4 transform, float alpha = 1, float delta = 0);

	const FontDescriptor& GetFontDescriptor() const;
	void SetFontDescriptor(const FontDescriptor& fontDescriptor);

	const char* GetString() const { return _string.c_str(); }
	void SetString(const char* value) { _string = value; }

	const float GetAlpha() const { return _alpha; }
	void SetAlpha(float value) { _alpha = value; }

	const float GetDelta() const { return _delta; }
	void SetDelta(float value) { _delta = value; }

private:
	virtual void AppendVertices(std::vector<Vertex_2f_2f_1f>& vertices);

private:
	StringGlyph(const StringGlyph&) { }
	StringGlyph& operator=(const StringGlyph&) { return *this; }
};


#endif
