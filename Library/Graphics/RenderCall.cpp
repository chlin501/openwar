// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "RenderCall.h"
#include "FrameBuffer.h"


RenderCallUniformBase::RenderCallUniformBase(GLint location) :
	_location(location)
{
}


RenderCallUniformBase::~RenderCallUniformBase()
{
}


void RenderCallUniformBase::Assign(int value)
{
	glUniform1iv(_location, 1, (const GLint*)&value);
	CHECK_ERROR_GL();
}


void RenderCallUniformBase::Assign(float value)
{
	glUniform1fv(_location, 1, (const GLfloat*)&value);
	CHECK_ERROR_GL();
}


void RenderCallUniformBase::Assign(const glm::vec2& value)
{
	glUniform2fv(_location, 1, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


void RenderCallUniformBase::Assign(const glm::vec3& value)
{
	glUniform3fv(_location, 1, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


void RenderCallUniformBase::Assign(const glm::vec4& value)
{
	glUniform4fv(_location, 1, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


void RenderCallUniformBase::Assign(const glm::mat2& value)
{
	glUniformMatrix2fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


void RenderCallUniformBase::Assign(const glm::mat3& value)
{
	glUniformMatrix3fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


void RenderCallUniformBase::Assign(const glm::mat4& value)
{
	glUniformMatrix4fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


/***/


RenderCallTexture::RenderCallTexture(GLint location, GLenum texture) :
	_location(location),
	_texture(texture)
{
}


void RenderCallTexture::SetValue(Texture* value, const Sampler& sampler)
{
	_value = value;
	_sampler = sampler;
}


void RenderCallTexture::Assign()
{
	if (_value != nullptr)
	{
		_value->UpdateTexture();

		glActiveTexture(GL_TEXTURE0 + _texture);
		CHECK_ERROR_GL();
		glBindTexture(GL_TEXTURE_2D, _value->_id);
		CHECK_ERROR_GL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(_sampler.minFilter));
		CHECK_ERROR_GL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(_sampler.magFilter));
		CHECK_ERROR_GL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(_sampler.sAddressMode));
		CHECK_ERROR_GL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(_sampler.tAddressMode));
		CHECK_ERROR_GL();
	}
	glUniform1i(_location, _texture);
	CHECK_ERROR_GL();
}


/***/


RenderCallBase::RenderCallBase(ShaderProgram* shaderprogram) :
	_shaderProgram(shaderprogram)
{
}


RenderCallBase::~RenderCallBase()
{
	for (RenderCallUniformBase* uniform : _uniforms)
		delete uniform;
	for (RenderCallTexture* texture : _textures)
		delete texture;
}


void RenderCallBase::Render()
{
	std::pair<bool, GLint> oldFrameBuffer{};

	bool has_vertices = false;
	if (_vertices != nullptr)
	{
		_vertices->Update();
		has_vertices = _vertices->_vbo != 0 && _vertices->_count != 0;
	}

	if ((_clearBits || has_vertices) && _frameBuffer != nullptr)
	{
		oldFrameBuffer.first = true;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer.second);
		glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer->id);
	}

	if (_clearBits)
	{
		if (_clearBits & GL_DEPTH_BUFFER_BIT)
			glDepthMask(GL_TRUE);

		glClear(_clearBits);
		_clearBits = 0;
	}

	if (has_vertices)
	{
		glUseProgram(_shaderProgram->_program);

		for (RenderCallUniformBase* uniform : _uniforms)
			uniform->Assign();

		for (RenderCallTexture* texture : _textures)
			texture->Assign();

		glUseProgram(_shaderProgram->_program);
		CHECK_ERROR_GL();

		if (_vertices->_vbo != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, _vertices->_vbo);
			CHECK_ERROR_GL();
		}

		for (const RenderCallAttribute& attribute : _attributes)
		{
			if (attribute._index != -1)
			{
				GLuint index = (GLuint)attribute._index;

				glEnableVertexAttribArray(index);
				CHECK_ERROR_GL();

				const GLvoid* pointer = reinterpret_cast<const GLvoid*>(attribute._offset);

				glVertexAttribPointer(index, attribute._size, attribute._type, GL_FALSE, attribute._stride, pointer);
				CHECK_ERROR_GL();
			}
		}

		if (_shaderProgram->_blend_sfactor != GL_ONE || _shaderProgram->_blend_dfactor != GL_ZERO)
		{
			glEnable(GL_BLEND);
			CHECK_ERROR_GL();
			glBlendFunc(_shaderProgram->_blend_sfactor, _shaderProgram->_blend_dfactor);
			CHECK_ERROR_GL();
		}

		if (_lineWidth != 0)
			glLineWidth(_lineWidth);

		if (_depthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		glDepthMask(static_cast<GLboolean>(_depthMask));

		glDrawArrays(_vertices->_mode, 0, _vertices->_count);
		CHECK_ERROR_GL();

		if (_shaderProgram->_blend_sfactor != GL_ONE || _shaderProgram->_blend_dfactor != GL_ZERO)
		{
			glDisable(GL_BLEND);
			CHECK_ERROR_GL();
			glBlendFunc(GL_ONE, GL_ZERO);
			CHECK_ERROR_GL();
		}

		for (const RenderCallAttribute& attribute : _attributes)
		{
			glDisableVertexAttribArray(attribute._index);
			CHECK_ERROR_GL();
		}

		if (_vertices->_vbo != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			CHECK_ERROR_GL();
		}
	}

	if (oldFrameBuffer.first)
		glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer.second);
};


RenderCallTexture* RenderCallBase::GetTexture(const char* name)
{
	RenderCallTexture* result = 0;
	GLint location = glGetUniformLocation(_shaderProgram->_program, name);
	for (RenderCallTexture* texture : _textures)
		if (texture->_location == location)
		{
			result = texture;
			break;
		}
	if (result == nullptr)
	{
		result = new RenderCallTexture(location, (GLenum)_texture_count++);
		_textures.push_back(result);
	}
	return result;
}
