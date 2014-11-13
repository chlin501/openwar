// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RenderCall_H
#define RenderCall_H

#include "GraphicsContext.h"


template <class T> GLint GetVertexAttributeSize();
template <class T> GLenum GetVertexAttributeType();


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

	void Render();

protected:
	RenderCallTexture* GetTexture(const char* name);
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

	template <class T1>
	RenderCall<ShaderProgramT>& SetVertices(VertexBuffer<Vertex1<T1>>* vertices, const char* name1)
	{
		_vertices = vertices;
		_attributes.push_back(MakeRenderCallAttribute<Vertex1<T1>, T1>(name1, &((Vertex1<T1>*)nullptr)->_1));
		return *this;
	}

	template <class T1, class T2>
	RenderCall<ShaderProgramT>& SetVertices(VertexBuffer<Vertex2<T1, T2>>* vertices, const char* name1, const char* name2)
	{
		_vertices = vertices;
		_attributes.push_back(MakeRenderCallAttribute<Vertex2<T1, T2>, T1>(name1, &((Vertex2<T1, T2>*)nullptr)->_1));
		_attributes.push_back(MakeRenderCallAttribute<Vertex2<T1, T2>, T2>(name2, &((Vertex2<T1, T2>*)nullptr)->_2));
		return *this;
	}

	template <class T1, class T2, class T3>
	RenderCall<ShaderProgramT>& SetVertices(VertexBuffer<Vertex3<T1, T2, T3>>* vertices, const char* name1, const char* name2, const char* name3)
	{
		_vertices = vertices;
		_attributes.push_back(MakeRenderCallAttribute<Vertex3<T1, T2, T3>, T1>(name1, &((Vertex3<T1, T2, T3>*)nullptr)->_1));
		_attributes.push_back(MakeRenderCallAttribute<Vertex3<T1, T2, T3>, T2>(name2, &((Vertex3<T1, T2, T3>*)nullptr)->_2));
		_attributes.push_back(MakeRenderCallAttribute<Vertex3<T1, T2, T3>, T3>(name3, &((Vertex3<T1, T2, T3>*)nullptr)->_3));
		return *this;
	}

	template <class T1, class T2, class T3, class T4>
	RenderCall<ShaderProgramT>& SetVertices(VertexBuffer<Vertex4<T1, T2, T3, T4>>* vertices, const char* name1, const char* name2, const char* name3, const char* name4)
	{
		_vertices = vertices;
		_attributes.push_back(MakeRenderCallAttribute<Vertex4<T1, T2, T3, T4>, T1>(name1, &((Vertex4<T1, T2, T3, T4>*)nullptr)->_1));
		_attributes.push_back(MakeRenderCallAttribute<Vertex4<T1, T2, T3, T4>, T2>(name2, &((Vertex4<T1, T2, T3, T4>*)nullptr)->_2));
		_attributes.push_back(MakeRenderCallAttribute<Vertex4<T1, T2, T3, T4>, T3>(name3, &((Vertex4<T1, T2, T3, T4>*)nullptr)->_3));
		_attributes.push_back(MakeRenderCallAttribute<Vertex4<T1, T2, T3, T4>, T4>(name4, &((Vertex4<T1, T2, T3, T4>*)nullptr)->_4));
		return *this;
	}

private:
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

	template <class VertexT, class AttributeT>
	RenderCallAttribute MakeRenderCallAttribute(const char* name, void* offset)
	{
		return RenderCallAttribute(
			glGetAttribLocation(_shaderprogram->_program, name),
			GetVertexAttributeSize<AttributeT>(),
			GetVertexAttributeType<AttributeT>(),
			sizeof(VertexT),
			(GLintptr)offset
		);
	}
};


#endif
