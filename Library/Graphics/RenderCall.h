#ifndef RenderCall_H
#define RenderCall_H

#include "GraphicsContext.h"

class RenderCallBase;


class UniformBase
{
	friend class RenderCallBase;

	GLint _location;
	GLenum _texture;

protected:
	UniformBase(GLint location);
	virtual ~UniformBase();

	virtual void Assign() = 0;

	void Assign(int value);
	void Assign(float value);
	void Assign(const glm::vec2& value);
	void Assign(const glm::vec3& value);
	void Assign(const glm::vec4& value);
	void Assign(const glm::mat2& value);
	void Assign(const glm::mat3& value);
	void Assign(const glm::mat4& value);
	void Assign(const texturex* value);
};


template <class T>
class Uniform : public UniformBase
{
	friend class RenderCallBase;

	T _value;

	Uniform(GLint location) : UniformBase(location)
	{
	}

	virtual void Assign()
	{
		UniformBase::Assign(_value);
	}
};


class RenderCallBase
{
protected:
	ShaderProgramBase* _shaderprogram;
	std::vector<UniformBase*> _uniforms;
	int _textures;

public:
	RenderCallBase(ShaderProgramBase* shaderprogram);
	virtual ~RenderCallBase();

	template <class T>
	RenderCallBase& SetUniform(const char* name, const T& value)
	{
		GetUniform<T>(name)->_value = value;
		return *this;
	}

	void Render();

private:
	template <class T>
	Uniform<T>* GetUniform(const char* name)
	{
		Uniform<T>* result = 0;
		GLint location = glGetUniformLocation(_shaderprogram->_program, name);
		for (UniformBase* uniform : _uniforms)
			if (uniform->_location == location)
			{
				result = dynamic_cast<Uniform<T>*>(uniform);
				break;
			}
		if (result == nullptr)
		{
			result = new Uniform<T>(location);
			if (typeid(T) == typeid(texturex*) || typeid(T) == typeid(const texturex*))
				result->_texture = (GLenum)_textures++;
			_uniforms.push_back(result);
		}
		return result;
	}

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

	RenderCall<_ShaderProgram>& SetVertices(VertexBuffer<VertexT>* vertices)
	{
		_vertices = vertices;
		return *this;
	}

private:

	virtual VertexBufferBase* GetVertexBufferBase()
	{
		return _vertices;
	}

};


#endif
