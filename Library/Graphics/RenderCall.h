// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef RenderCall_H
#define RenderCall_H

#include "GraphicsContext.h"
#include "ShaderProgram.h"
#include "Vertex.h"
#include "Sampler.h"

class FrameBuffer;


enum class CullFace { None, Front, Back, FrontAndBack };


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
	Sampler _sampler;

protected:
	RenderCallTexture(GLint location, GLenum texture);

	void SetValue(Texture* value, const Sampler& sampler);
	void Assign();
};


class RenderCallBase
{
protected:
	ShaderProgram* _shaderProgram;
	FrameBuffer* _frameBuffer{};
	std::vector<RenderCallUniformBase*> _uniforms;
	std::vector<RenderCallTexture*> _textures;
	std::vector<RenderCallAttribute> _attributes;
	VertexBufferBase* _vertices{};
	int _texture_count{};
	GLbitfield _clearBits{};
	glm::vec4 _clearColor;
	GLfloat _lineWidth{};
	CullFace _cullFace{CullFace::Back};
	bool _depthTest{};
	bool _depthMask{};

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
		GLint location = glGetUniformLocation(_shaderProgram->_program, name);
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
			glGetAttribLocation(_shaderProgram->_program, traits.name),
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

	RenderCall<ShaderProgramT>& ClearDepth()
	{
		_clearBits |= GL_DEPTH_BUFFER_BIT;
		return *this;
	}

	RenderCall<ShaderProgramT>& ClearColor(const glm::vec4& value)
	{
		_clearBits |= GL_COLOR_BUFFER_BIT;
		_clearColor = value;
		return *this;
	}

	RenderCall<ShaderProgramT>& SetCullFace(CullFace value)
	{
		_cullFace = value;
		switch (value)
		{
			case CullFace::None:
				glDisable(GL_CULL_FACE);
				break;
			case CullFace::Front:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;
			case CullFace::Back: // default value
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;
			case CullFace::FrontAndBack:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT_AND_BACK);
				break;
		}
		return *this;
	}


	RenderCall<ShaderProgramT>& SetDepthTest(bool value)
	{
		_depthTest = value;
		return *this;
	}

	RenderCall<ShaderProgramT>& SetDepthMask(bool value)
	{
		_depthMask = value;
		return *this;
	}


	RenderCall<ShaderProgramT>& SetFrameBuffer(FrameBuffer* frameBuffer)
	{
		_frameBuffer = frameBuffer;
		return *this;
	}

	template <class T>
	RenderCall<ShaderProgramT>& SetUniform(const char* name, const T& value)
	{
		GetUniform<T>(name)->_value = value;
		return *this;
	}

	RenderCall<ShaderProgramT>& SetTexture(const char* name, Texture* value, const Sampler& sampler = Sampler{})
	{
		GetTexture(name)->SetValue(value, sampler);
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

	RenderCall<ShaderProgramT>& SetLineWidth(GLfloat value)
	{
		_lineWidth = value;
		return *this;
	}
};


#endif
