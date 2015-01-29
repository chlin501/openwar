// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RenderCall_H
#define RenderCall_H

#include "GraphicsContext.h"
#include "ShaderProgram.h"
#include "Vertex.h"


struct RenderCallAttribute
{
	GLint _index;
	GLint _size;
	GLenum _type;
	GLsizei _stride;
	GLintptr _offset;

	RenderCallAttribute(GLint index, GLint size, GLenum type, GLsizei stride, GLintptr offset) :
		_index(index), _size(size), _type(type), _stride(stride), _offset(offset)
	{
	}
};


class RenderCallUniformBase
{
	friend class RenderCallBase;
	template <class _ShaderProgram> friend class RenderCall;

	GLint _location;

protected:
	RenderCallUniformBase(GLint location);
	virtual ~RenderCallUniformBase();

	virtual void Assign() = 0;

	void Assign(int value);
	void Assign(float value);
	void Assign(const glm::vec2& value);
	void Assign(const glm::vec3& value);
	void Assign(const glm::vec4& value);
	void Assign(const glm::mat2& value);
	void Assign(const glm::mat3& value);
	void Assign(const glm::mat4& value);
};


template <class T>
class RenderCallUniform : public RenderCallUniformBase
{
	friend class RenderCallBase;
	template <class _ShaderProgram> friend class RenderCall;

	T _value;

	RenderCallUniform(GLint location) : RenderCallUniformBase(location)
	{
	}

	virtual void Assign()
	{
		RenderCallUniformBase::Assign(_value);
	}
};


class RenderCallTexture
{
	friend class RenderCallBase;
	template <class _ShaderProgram> friend class RenderCall;

	GLint _location;
	GLenum _texture;
	Texture* _value;

protected:
	RenderCallTexture(GLint location, GLenum texture);

	void Assign();
};


class RenderCallBase
{
protected:
	ShaderProgram* _shaderprogram;
	std::vector<RenderCallUniformBase*> _uniforms;
	std::vector<RenderCallTexture*> _textures;
	std::vector<RenderCallAttribute> _attributes;
	VertexBufferBase* _vertices;
	int _texture_count;

public:
	RenderCallBase(ShaderProgram* shaderprogram);
	virtual ~RenderCallBase();

	RenderCallBase(const RenderCallBase&) = delete;
	RenderCallBase& operator=(const RenderCallBase&) = delete;

	void Render();

protected:
	template <class T>
	RenderCallUniform<T>* GetUniform(const char* name)
	{
		RenderCallUniform<T>* result = 0;
		GLint location = glGetUniformLocation(_shaderprogram->_program, name);
		for (RenderCallUniformBase* uniform : _uniforms)
			if (uniform->_location == location)
			{
				result = dynamic_cast<RenderCallUniform<T>*>(uniform);
				break;
			}
		if (result == nullptr)
		{
			result = new RenderCallUniform<T>(location);
			_uniforms.push_back(result);
		}
		return result;
	}

	RenderCallTexture* GetTexture(const char* name);

	RenderCallAttribute MakeRenderCallAttribute(const VertexAttributeTraits& traits, GLsizei stride)
	{
		return RenderCallAttribute{
			glGetAttribLocation(_shaderprogram->_program, traits.name),
			traits.size,
			traits.type,
			stride,
			traits.offset
		};
	}
};


template <class _ShaderProgram>
class RenderCall : public RenderCallBase
{
public:
	typedef _ShaderProgram ShaderProgramT;

	RenderCall(GraphicsContext* gc) : RenderCallBase(gc->GetShaderProgram<_ShaderProgram>())
	{
	}

	template <class T>
	RenderCall<ShaderProgramT>& SetUniform(const char* name, const T& value)
	{
		GetUniform<T>(name)->_value = value;
		return *this;
	}

	RenderCall<ShaderProgramT>& SetTexture(const char* name, Texture* value)
	{
		GetTexture(name)->_value = value;
		return *this;
	}

	template <typename VertexT, typename... N>
	RenderCall<ShaderProgramT>& SetVertices(VertexBuffer<VertexT>* vertices, N... names)
	{
		static_assert(VertexT::count == sizeof...(N), "incorrect number of names");

		_vertices = vertices;
		_attributes.clear();

		GLsizei stride = static_cast<GLsizei>(sizeof(VertexT));
		for (const auto& traits : VertexT::GetVertexAttributeTraits(names...))
			_attributes.push_back(MakeRenderCallAttribute(traits, stride));

		return *this;
	}
};


#endif
