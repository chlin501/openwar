// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef VERTEXSHAPE_H
#define VERTEXSHAPE_H

#include <functional>
#include <vector>

#include "VertexBuffer.h"


class glyphversion
{
	static int _highest_version;

public:
	int value;

	glyphversion() : value(++_highest_version) { }
	void increase() { value = ++_highest_version; }
};


template <class T1>
struct vertexglyph1
{
	typedef std::function<void(std::vector<Vertex1<T1>>& vertices)> generator_type;

	generator_type generator;
	glyphversion* version;

	vertexglyph1() : generator(), version(nullptr) { }
	vertexglyph1(generator_type g) : generator(g), version(nullptr) { }
	vertexglyph1(generator_type g, glyphversion* v) : generator(g), version(v) { }
};


template <class T1, class T2>
struct vertexglyph2
{
	typedef std::function<void(std::vector<Vertex2<T1, T2>>& vertices)> generator_type;

	generator_type generator;
	glyphversion* version;

	vertexglyph2() : generator(), version(nullptr) { }
	vertexglyph2(generator_type g) : generator(g), version(nullptr) { }
	vertexglyph2(generator_type g, glyphversion* v) : generator(g), version(v) { }
};


template <class T1, class T2, class T3>
struct vertexglyph3
{
	typedef std::function<void(std::vector<Vertex3<T1, T2, T3>>& vertices)> generator_type;

	generator_type generator;
	glyphversion* version;

	vertexglyph3() : generator(), version(nullptr) { }
	vertexglyph3(generator_type g) : generator(g), version(nullptr) { }
	vertexglyph3(generator_type g, glyphversion* v) : generator(g), version(v) { }
};


template <class T1, class T2, class T3, class T4>
struct vertexglyph4
{
	typedef std::function<void(std::vector<Vertex4<T1, T2, T3, T4>>& vertices)> generator_type;

	generator_type generator;
	glyphversion* version;

	vertexglyph4() : generator(), version(nullptr) { }
	vertexglyph4(generator_type g) : generator(g), version(nullptr) { }
	vertexglyph4(generator_type g, glyphversion* v) : generator(g), version(v) { }
};


template <class T1>
class vertexshape1
{
	typedef Vertex1<T1> vertex_type;
	typedef vertexglyph1<T1> vertexglyph_type;

	VertexBuffer<vertex_type> _vbo;
	int _version;

public:
	std::vector<vertexglyph_type> glyphs;

	vertexshape1() : _vbo(), _version(0) { }

	bool needs_update() const
	{
		/*int version = 0;
		for (const vertexglyph_type& glyph : glyphs)
		{
			if (glyph.version == nullptr)
				return true;
			version += glyph.version->value;
		}*/
		return true;//version != _version;
	}

	VertexBuffer<vertex_type>& update_vbo()
	{
		if (needs_update())
		{
			_vbo._vertices.clear();
			_version = 0;
			for (vertexglyph_type& glyph : glyphs)
			{
				if (glyph.generator)
					glyph.generator(_vbo._vertices);
				if (glyph.version != nullptr)
					_version += glyph.version->value;
			}
			_vbo.update(GL_STATIC_DRAW);
		}
		return _vbo;
	}
};


template <class T1, class T2>
class vertexshape2
{
	typedef Vertex2<T1, T2> vertex_type;
	typedef vertexglyph2<T1, T2> vertexglyph_type;

	VertexBuffer<vertex_type> _vbo;
	int _version;

public:
	std::vector<vertexglyph_type> glyphs;

	vertexshape2() : _vbo(), _version(0) { }

	bool needs_update() const
	{
		/*int version = 0;
		for (const vertexglyph_type& glyph : glyphs)
		{
			if (glyph.version == nullptr)
				return true;
			version += glyph.version->value;
		}*/
		return true;//version != _version;
	}

	VertexBuffer<vertex_type>& update_vbo()
	{
		if (needs_update())
		{
			_vbo._vertices.clear();
			_version = 0;
			for (vertexglyph_type& glyph : glyphs)
			{
				if (glyph.generator)
					glyph.generator(_vbo._vertices);
				if (glyph.version != nullptr)
					_version += glyph.version->value;
			}
			_vbo.update(GL_STATIC_DRAW);
		}
		return _vbo;
	}
};


template <class T1, class T2, class T3>
class vertexshape3
{
public:
	typedef Vertex3<T1, T2, T3> vertex_type;
	typedef vertexglyph3<T1, T2, T3> vertexglyph_type;

	VertexBuffer<vertex_type> _vbo;
	int _version;

public:
	std::vector<vertexglyph_type> glyphs;

	vertexshape3() : _vbo(), _version(0) { }

	bool needs_update() const
	{
		/*int version = 0;
		for (const vertexglyph_type& glyph : glyphs)
		{
			if (glyph.version == nullptr)
				return true;
			version += glyph.version->value;
		}*/
		return true; //version != _version;
	}

	VertexBuffer<vertex_type>& update_vbo()
	{
		if (needs_update())
		{
			_vbo._vertices.clear();
			_version = 0;
			for (vertexglyph_type& glyph : glyphs)
			{
				if (glyph.generator)
					glyph.generator(_vbo._vertices);
				if (glyph.version != nullptr)
					_version += glyph.version->value;
			}
			_vbo.update(GL_STATIC_DRAW);
		}
		return _vbo;
	}
};


template <class T1, class T2, class T3, class T4>
class vertexshape4
{
	typedef Vertex4<T1, T2, T3, T4> vertex_type;
	typedef vertexglyph4<T1, T2, T3, T4> vertexglyph_type;

	VertexBuffer<vertex_type> _vbo;
	int _version;

public:
	std::vector<vertexglyph_type> glyphs;

	vertexshape4() : _vbo(), _version(0) { }

	bool needs_update() const
	{
		/*int version = 0;
		for (const vertexglyph_type& glyph : glyphs)
		{
			if (glyph.version == nullptr)
				return true;
			version += glyph.version->value;
		}*/
		return true;//version != _version;
	}

	VertexBuffer<vertex_type>& update_vbo()
	{
		if (needs_update())
		{
			_vbo._vertices.clear();
			_version = 0;
			for (vertexglyph_type& glyph : glyphs)
			{
				if (glyph.generator)
					glyph.generator(_vbo._vertices);
				if (glyph.version != nullptr)
					_version += glyph.version->value;
			}
			_vbo.update(GL_STATIC_DRAW);
		}
		return _vbo;
	}
};


#endif
