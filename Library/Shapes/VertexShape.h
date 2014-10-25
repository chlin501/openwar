#ifndef VertexShape_H
#define VertexShape_H

#include "VertexBuffer.h"
#include <functional>
#include <vector>

class Shape;
template <class _Vertex> class VertexBuffer;
template <class _Vertex> class VertexShape;


class VertexGlyphBase
{
	Shape* _shape;

public:
	VertexGlyphBase(Shape* shape);
	~VertexGlyphBase();

protected:
	VertexGlyphBase(const VertexGlyphBase&) { }
	VertexGlyphBase& operator=(VertexGlyphBase&) { return *this; }
};


template <class _Vertex>
class VertexGlyph : public VertexGlyphBase
{
	friend class VertexShape<_Vertex>;
	VertexShape<_Vertex>* _vertexBuffer;

public:
	typedef _Vertex VertexType;
	typedef std::function<void(std::vector<_Vertex>&)> RebuildType;

	RebuildType _rebuild;

	VertexGlyph() : VertexGlyphBase(nullptr), _vertexBuffer(nullptr), _rebuild() { }
	VertexGlyph(RebuildType rebuild) : VertexGlyphBase(nullptr), _vertexBuffer(nullptr), _rebuild(rebuild) { }

	~VertexGlyph()
	{
		if (_vertexBuffer != nullptr)
			_vertexBuffer->RemoveGlyph(this);
	}

private:
	VertexGlyph(const VertexGlyph<VertexType>& _) : VertexGlyphBase(_) { }
	VertexGlyph<VertexType>& operator=(VertexGlyph<VertexType>&) { return *this; }
};



template <class _Vertex>
class VertexShapeBase : public VertexBuffer<_Vertex>
{
public:
	typedef _Vertex VertexT;

	std::vector<VertexT> _vertices;

	VertexShapeBase() { }

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
class VertexShape : public VertexShapeBase<_Vertex>
{
	friend class VertexGlyph<_Vertex>;
	std::vector<VertexGlyph<_Vertex>*> _glyphs;

public:
	typedef _Vertex VertexT;

	VertexShape() { }

	void ClearGlyphs()
	{
		for (VertexGlyph<VertexT>* glyph : _glyphs)
			glyph->_vertexBuffer = nullptr;
		_glyphs.clear();
	}

	void AddGlyph(VertexGlyph<VertexT>* glyph)
	{
		if (glyph->_vertexBuffer != nullptr)
			glyph->_vertexBuffer->RemoveGlyph(glyph);
		glyph->_vertexBuffer = this;
		_glyphs.push_back(glyph);
	}

	void RemoveGlyph(VertexGlyph<VertexT>* glyph)
	{
		glyph->_vertexBuffer = nullptr;
		_glyphs.erase(
			std::find(_glyphs.begin(), _glyphs.end(), glyph),
			_glyphs.end());
	}

	VertexBuffer<VertexT>& UpdateVBOFromGlyphs()
	{
		VertexShapeBase<VertexT>::_vertices.clear();
		for (VertexGlyph<VertexT>* glyph : _glyphs)
		{
			if (glyph->_rebuild)
				glyph->_rebuild(VertexShapeBase<VertexT>::_vertices);
		}
		this->UpdateVBO(GL_STATIC_DRAW);
		return *this;
	}
};


typedef VertexShape<Vertex_2f> VertexShape_2f;
typedef VertexShape<Vertex_3f> VertexShape_3f;

typedef VertexShape<Vertex_2f_2f> VertexShape_2f_2f;
typedef VertexShape<Vertex_2f_4f> VertexShape_2f_4f;
typedef VertexShape<Vertex_3f_1f> VertexShape_3f_1f;
typedef VertexShape<Vertex_3f_2f> VertexShape_3f_2f;
typedef VertexShape<Vertex_3f_3f> VertexShape_3f_3f;
typedef VertexShape<Vertex_3f_4f> VertexShape_3f_4f;

typedef VertexShape<Vertex_2f_2f_1f> VertexShape_2f_2f_1f;
typedef VertexShape<Vertex_3f_4f_1f> VertexShape_3f_4f_1f;
typedef VertexShape<Vertex_2f_2f_2f> VertexShape_2f_2f_2f;

typedef VertexShape<Vertex_3f_1f_2f_2f> VertexShape_3f_1f_2f_2f;


#endif
