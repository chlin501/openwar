// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "RenderCall.h"
#include "FrameBuffer.h"
#include "Viewport.h"


RenderCallUniformBase::RenderCallUniformBase(GLint location) :
	_location(location)
{
}


RenderCallUniformBase::~RenderCallUniformBase()
{
}


void RenderCallUniformBase::Assign(int value)
{
	if (_location != -1)
	{
		glUniform1iv(_location, 1, (const GLint*)&value);
        CHECK_OPENGL_ERROR();
	}
	else
	{
		// TODO: should log error
	}
}


void RenderCallUniformBase::Assign(float value)
{
	glUniform1fv(_location, 1, (const GLfloat*)&value);
    CHECK_OPENGL_ERROR();
}


void RenderCallUniformBase::Assign(const glm::vec2& value)
{
	glUniform2fv(_location, 1, reinterpret_cast<const GLfloat*>(&value));
    CHECK_OPENGL_ERROR();
}


void RenderCallUniformBase::Assign(const glm::vec3& value)
{
	glUniform3fv(_location, 1, reinterpret_cast<const GLfloat*>(&value));
    CHECK_OPENGL_ERROR();
}


void RenderCallUniformBase::Assign(const glm::vec4& value)
{
	glUniform4fv(_location, 1, reinterpret_cast<const GLfloat*>(&value));
    CHECK_OPENGL_ERROR();
}


void RenderCallUniformBase::Assign(const glm::mat2& value)
{
	glUniformMatrix2fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&value));
    CHECK_OPENGL_ERROR();
}


void RenderCallUniformBase::Assign(const glm::mat3& value)
{
	glUniformMatrix3fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&value));
    CHECK_OPENGL_ERROR();
}


void RenderCallUniformBase::Assign(const glm::mat4& value)
{
	glUniformMatrix4fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&value));
    CHECK_OPENGL_ERROR();
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
	if (_value)
	{
		_value->UpdateTexture();

		glActiveTexture(GL_TEXTURE0 + _texture);
        CHECK_OPENGL_ERROR();
		glBindTexture(GL_TEXTURE_2D, _value->_id);
        CHECK_OPENGL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(_sampler.minFilter));
        CHECK_OPENGL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(_sampler.magFilter));
        CHECK_OPENGL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(_sampler.sAddressMode));
        CHECK_OPENGL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(_sampler.tAddressMode));
        CHECK_OPENGL_ERROR();
	}
	glUniform1i(_location, (GLint)_texture);
    CHECK_OPENGL_ERROR();
}


/***/


RenderCallBase::RenderCallBase(GraphicsContext* gc, ShaderProgram* shaderprogram) :
	_gc(gc),
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


void RenderCallBase::Render(const Viewport& viewport)
{
	GLint oldViewport[4]{};
	glGetIntegerv(GL_VIEWPORT, oldViewport);
	float scaling = viewport.GetFrameBuffer() ? 1 : _gc->GetCombinedScaling();
	bounds2i b = (bounds2i)((bounds2f)viewport.GetViewportBounds() * scaling);
	glViewport(b.min.x, b.min.y, b.x().size(), b.y().size());

	bool has_vertices = false;
	if (_vertices)
	{
		_vertices->Update();
		has_vertices = _vertices->_vbo != 0 && _vertices->_count != 0;
	}

	std::pair<bool, GLuint> oldFrameBuffer{};

	FrameBuffer* frameBuffer = viewport.GetFrameBuffer();
	if ((_clearBits || has_vertices) && frameBuffer)
	{
		GLint oldFrameBufferId{};
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBufferId);
		oldFrameBuffer = std::make_pair(true, static_cast<GLuint>(oldFrameBufferId));
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->_id);
#ifdef OPENWAR_PLATFORM_MAC
		if (!frameBuffer->HasColor())
			glDrawBuffer(GL_NONE);
#endif
	};

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
        CHECK_OPENGL_ERROR();

		if (_vertices->_vbo != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, _vertices->_vbo);
            CHECK_OPENGL_ERROR();
		}

		for (const RenderCallAttribute& attribute : _attributes)
		{
			if (attribute._index != -1)
			{
				GLuint index = static_cast<GLuint>(attribute._index);

				glEnableVertexAttribArray(index);
                CHECK_OPENGL_ERROR();

				const GLvoid* pointer = reinterpret_cast<const GLvoid*>(attribute._offset);

				glVertexAttribPointer(index, attribute._size, attribute._type, GL_FALSE, attribute._stride, pointer);
                CHECK_OPENGL_ERROR();
			}
		}

		if (_shaderProgram->_blend_sfactor != GL_ONE || _shaderProgram->_blend_dfactor != GL_ZERO)
		{
			glEnable(GL_BLEND);
            CHECK_OPENGL_ERROR();
			glBlendFunc(_shaderProgram->_blend_sfactor, _shaderProgram->_blend_dfactor);
            CHECK_OPENGL_ERROR();
		}

		if (_lineWidth != 0)
			glLineWidth(_lineWidth);

		if (_depthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		glDepthMask(static_cast<GLboolean>(_depthMask));

		if (_cullBack)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);

		glDrawArrays(_vertices->_mode, 0, _vertices->_count);
        CHECK_OPENGL_ERROR();

		if (_shaderProgram->_blend_sfactor != GL_ONE || _shaderProgram->_blend_dfactor != GL_ZERO)
		{
			glDisable(GL_BLEND);
            CHECK_OPENGL_ERROR();
			glBlendFunc(GL_ONE, GL_ZERO);
            CHECK_OPENGL_ERROR();
		}

		for (const RenderCallAttribute& attribute : _attributes)
		{
			if (attribute._index != -1)
			{
				glDisableVertexAttribArray(static_cast<GLuint>(attribute._index));
                CHECK_OPENGL_ERROR();
			}
		}

		if (_vertices->_vbo != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
            CHECK_OPENGL_ERROR();
		}
	}

	if (oldFrameBuffer.first)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer.second);
#ifdef OPENWAR_PLATFORM_MAC
		if (!frameBuffer->HasColor())
			glDrawBuffer(GL_BACK);
#endif
	}

	glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
}


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

	if (!result)
	{
		result = new RenderCallTexture(location, (GLenum)_texture_count++);
		_textures.push_back(result);
	}

	return result;
}
