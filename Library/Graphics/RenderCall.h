#ifndef RenderCall_H
#define RenderCall_H

#include "GraphicsContext.h"
#include "Renderable.h"

class RenderCallBase;


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

	GLint _location;
	GLenum _texture;
	Texture* _value;

protected:
	RenderCallTexture(GLint location, GLenum texture);

	void Assign();
};


class RenderCallBase : public Renderable
{
protected:
	ShaderProgramBase* _shaderprogram;
	std::vector<RenderCallUniformBase*> _uniforms;
	std::vector<RenderCallTexture*> _textures;
	std::vector<RenderCallAttribute> _attributes;
	int _texture_count;

public:
	RenderCallBase(ShaderProgramBase* shaderprogram);
	virtual ~RenderCallBase();

	template <class T>
	RenderCallBase& SetUniform(const char* name, const T& value)
	{
		GetUniform<T>(name)->_value = value;
		return *this;
	}

	RenderCallBase& SetTexture(const char* name, Texture* value)
	{
		GetTexture(name)->_value = value;
		return *this;
	}

	// Renderable

	virtual void Render();
	virtual bool HasChangedSinceLastRender() const;

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

	RenderCallTexture* GetTexture(const char* name);

	virtual VertexBufferBase* GetVertexBufferBase() = 0;
};


template <class _ShaderProgram>
class RenderCall : public RenderCallBase
{
public:
	typedef typename _ShaderProgram::VertexT VertexT;

	VertexBuffer<VertexT>* _vertices;

	RenderCall(GraphicsContext* gc) : RenderCallBase(gc->GetShaderProgram<_ShaderProgram>()),
		_vertices(nullptr)
	{
	}

	RenderCall<_ShaderProgram>& SetVertices(VertexBuffer<VertexT>* vertices, const char* name1)
	{
		_vertices = vertices;
		_attributes.push_back(MakeRenderCallAttribute<typename VertexT::T1>(name1, &((VertexT*)nullptr)->_1));
		return *this;
	}

	RenderCall<_ShaderProgram>& SetVertices(VertexBuffer<VertexT>* vertices, const char* name1, const char* name2)
	{
		_vertices = vertices;
		_attributes.push_back(MakeRenderCallAttribute<typename VertexT::T1>(name1, &((VertexT*)nullptr)->_1));
		_attributes.push_back(MakeRenderCallAttribute<typename VertexT::T2>(name2, &((VertexT*)nullptr)->_2));
		return *this;
	}

	RenderCall<_ShaderProgram>& SetVertices(VertexBuffer<VertexT>* vertices, const char* name1, const char* name2, const char* name3)
	{
		_vertices = vertices;
		_attributes.push_back(MakeRenderCallAttribute<typename VertexT::T1>(name1, &((VertexT*)nullptr)->_1));
		_attributes.push_back(MakeRenderCallAttribute<typename VertexT::T2>(name2, &((VertexT*)nullptr)->_2));
		_attributes.push_back(MakeRenderCallAttribute<typename VertexT::T3>(name3, &((VertexT*)nullptr)->_3));
		return *this;
	}

	RenderCall<_ShaderProgram>& SetVertices(VertexBuffer<VertexT>* vertices, const char* name1, const char* name2, const char* name3, const char* name4)
	{
		_vertices = vertices;
		_attributes.push_back(MakeRenderCallAttribute<typename VertexT::T1>(name1, &((VertexT*)nullptr)->_1));
		_attributes.push_back(MakeRenderCallAttribute<typename VertexT::T2>(name2, &((VertexT*)nullptr)->_2));
		_attributes.push_back(MakeRenderCallAttribute<typename VertexT::T3>(name3, &((VertexT*)nullptr)->_3));
		_attributes.push_back(MakeRenderCallAttribute<typename VertexT::T4>(name4, &((VertexT*)nullptr)->_4));
		return *this;
	}

private:
	template <class AttributeT>
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

	virtual VertexBufferBase* GetVertexBufferBase()
	{
		return _vertices;
	}
};


#endif
