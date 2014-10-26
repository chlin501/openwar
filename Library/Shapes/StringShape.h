#ifndef STRINGSHAPE_H
#define STRINGSHAPE_H

#include <map>
#include <glm/gtc/matrix_transform.hpp>

#ifndef OPENWAR_USE_SDL
#if TARGET_OS_IPHONE
#define OPENWAR_USE_UIFONT
#else
#define OPENWAR_USE_NSFONT
#endif
#endif

#ifdef OPENWAR_USE_SDL
#include <SDL2_ttf/SDL_ttf.h>
#endif

#ifdef OPENWAR_USE_UIFONT
#import <UIKit/UIKit.h>
#endif

#ifdef OPENWAR_USE_NSFONT
#import <AppKit/AppKit.h>
#endif

#include "Algebra/bounds.h"
#include "ShaderProgram.h"
#include "texture.h"
#include "Shape.h"

class GraphicsContext;


class StringShader : public ShaderProgram3<glm::vec2, glm::vec2, float>
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
	StringShader(GraphicsContext* gc);
};


struct StringFont
{
#ifdef OPENWAR_USE_SDL
	typedef TTF_Font* font_ptr;
#endif
#ifdef OPENWAR_USE_UIFONT
	typedef UIFont* font_ptr;
#endif
#ifdef OPENWAR_USE_NSFONT
	typedef NSFont* font_ptr;
#endif

	struct item
	{
		font_ptr _font;
#ifdef OPENWAR_USE_SDL
		std::string _string;
#else
		NSString* _string;
#endif
		glm::vec2 _bounds_origin;
		glm::vec2 _bounds_size;
		float _u0, _u1;
		float _v0, _v1;
	};

	static Image* _image;

#ifdef OPENWAR_USE_SDL
	font_ptr _font1;
	font_ptr _font2;
	font_ptr _emoji;
#else
	font_ptr _font;
#endif

	float _pixelDensity;
	texture _texture;

	std::map<std::string, item> _items;
	glm::vec2 _next;
	bool _dirty;

public:
	StringFont(GraphicsContext* gc, const char* name, float size, float pixelDensity);
	StringFont(GraphicsContext* gc, bool bold, float size, float pixelDensity);
	~StringFont();

private:
	void initialize();

public:
	font_ptr get_font_ptr() const;
	font_ptr get_font_ptr(wchar_t wc) const;

	item add_character(font_ptr font, const std::string& character);

	void update_texture();

	glm::vec2 measure(const char* text);
	glm::vec2 get_size(const item& item) const;

private:
	StringFont(const StringFont&) : _texture(nullptr) { }
	StringFont& operator=(const StringFont&) { return *this; }
};


template <class _Vertex> class VertexShapeX;


template <class _Vertex>
class VertexGlyphX
{
	friend class VertexShapeX<_Vertex>;
	VertexShapeX<_Vertex>* _vertexBuffer;

public:
	typedef _Vertex VertexType;
	typedef std::function<void(std::vector<_Vertex>&)> RebuildType;

	RebuildType _rebuild;

	VertexGlyphX() : _vertexBuffer(nullptr), _rebuild() { }
	VertexGlyphX(RebuildType rebuild) : _vertexBuffer(nullptr), _rebuild(rebuild) { }

	~VertexGlyphX()
	{
		if (_vertexBuffer != nullptr)
			_vertexBuffer->RemoveGlyph(this);
	}

private:
	VertexGlyphX(const VertexGlyphX<VertexType>&) { }
	VertexGlyphX<VertexType>& operator=(VertexGlyphX<VertexType>&) { return *this; }
};


class StringGlyph
{
	VertexGlyphX<Vertex_2f_2f_1f> _glyph;
	std::string _string;
	glm::mat4x4 _transform;
	float _alpha;
	float _delta;

public:
	typedef Vertex_2f_2f_1f vertex_type;

	StringGlyph();
	StringGlyph(const char* string, glm::vec2 translate, float alpha = 1, float delta = 0);
	StringGlyph(const char* string, glm::mat4x4 transform, float alpha = 1, float delta = 0);

	const char* get_string() const { return _string.c_str(); }
	void set_string(const char* value) { _string = value; }

	const glm::mat4x4 get_transform() const { return _transform; }
	void set_transform(glm::mat4x4 value) { _transform = value; }
	void set_translate(glm::vec2 value) { _transform = glm::translate(glm::mat4(), glm::vec3(value, 0)); }

	const float get_alpha() const { return _alpha; }
	void set_alpha(float value) { _alpha = value; }

	const float get_delta() const { return _delta; }
	void set_delta(float value) { _delta = value; }

	VertexGlyphX<Vertex_2f_2f_1f>* GetGlyph(StringFont* font);

	void generate(StringFont* font, std::vector<vertex_type>& vertices);

private:
	StringGlyph(const StringGlyph&) { }
	StringGlyph& operator=(const StringGlyph&) { return *this; }
};



template <class _Vertex>
class VertexShapeBaseX : public VertexBuffer<_Vertex>
{
public:
	typedef _Vertex VertexT;

	std::vector<VertexT> _vertices;

	VertexShapeBaseX() { }

	void Reset(GLenum mode)
	{
		VertexBufferBase::_mode = mode;
		_vertices.clear();
	}

	void Clear()
	{
		_vertices.clear();
	}

	void AddVertex(const VertexT& vertex)
	{
		_vertices.push_back(vertex);
	}

	virtual void UpdateVBO(GLenum usage)
	{
		if (VertexBufferBase::_vbo == 0)
		{
			glGenBuffers(1, &this->_vbo);
			CHECK_ERROR_GL();
			if (VertexBufferBase::_vbo == 0)
				return;
		}

		GLsizeiptr size = sizeof(VertexT) * _vertices.size();
		const GLvoid* data = _vertices.data();

		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase::_vbo);
		CHECK_ERROR_GL();
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
		CHECK_ERROR_GL();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_ERROR_GL();

		VertexBufferBase::_count = (GLsizei)_vertices.size();
	}


	virtual const void* data() const
	{
		return _vertices.data();
	}

	virtual GLsizei count() const
	{
		return VertexBufferBase::_vbo != 0 ? VertexBufferBase::_count : (GLsizei)_vertices.size();
	}


};


template <class _Vertex>
class VertexShapeX : public VertexShapeBaseX<_Vertex>
{
	friend class VertexGlyphX<_Vertex>;
	std::vector<VertexGlyphX<_Vertex>*> _glyphs;

public:
	typedef _Vertex VertexT;

	VertexShapeX() { }

	void ClearGlyphs()
	{
		for (VertexGlyphX<VertexT>* glyph : _glyphs)
			glyph->_vertexBuffer = nullptr;
		_glyphs.clear();
	}

	void AddGlyph(VertexGlyphX<VertexT>* glyph)
	{
		if (glyph->_vertexBuffer != nullptr)
			glyph->_vertexBuffer->RemoveGlyph(glyph);
		glyph->_vertexBuffer = this;
		_glyphs.push_back(glyph);
	}

	void RemoveGlyph(VertexGlyphX<VertexT>* glyph)
	{
		glyph->_vertexBuffer = nullptr;
		_glyphs.erase(
			std::find(_glyphs.begin(), _glyphs.end(), glyph),
			_glyphs.end());
	}

	VertexBuffer<VertexT>& UpdateVBOFromGlyphs()
	{
		VertexShapeBaseX<VertexT>::_vertices.clear();
		for (VertexGlyphX<VertexT>* glyph : _glyphs)
		{
			if (glyph->_rebuild)
				glyph->_rebuild(VertexShapeBaseX<VertexT>::_vertices);
		}
		this->UpdateVBO(GL_STATIC_DRAW);
		return *this;
	}
};


class StringShape
{
	std::vector<StringGlyph*> _stringglyphs;

public:
	VertexShapeX<Vertex_2f_2f_1f> _vertices;
	StringFont* _font;

	explicit StringShape(StringFont* font);

	void clear();
	void add(const char* string, glm::mat4x4 transform, float alpha = 1, float delta = 0);

	void update(GLenum usage);

private:
	StringShape(const StringShape&) { }
	StringShape& operator=(const StringShape&) { return *this; }
};


#endif
