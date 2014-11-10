#ifndef RenderCall_H
#define RenderCall_H

#include "GraphicsContext.h"
#include "Renderable.h"

class RenderCallBase;


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
