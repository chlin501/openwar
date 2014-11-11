#include "RenderCall.h"


template<> GLint GetVertexAttributeSize<float>() { return 1; }
template<> GLint GetVertexAttributeSize<glm::vec2>() { return 2; }
template<> GLint GetVertexAttributeSize<glm::vec3>() { return 3; }
template<> GLint GetVertexAttributeSize<glm::vec4>() { return 4; }

template<> GLenum GetVertexAttributeType<float>() { return GL_FLOAT; }
template<> GLenum GetVertexAttributeType<glm::vec2>() { return GL_FLOAT; }
template<> GLenum GetVertexAttributeType<glm::vec3>() { return GL_FLOAT; }
template<> GLenum GetVertexAttributeType<glm::vec4>() { return GL_FLOAT; }


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


void RenderCallTexture::Assign()
{
	if (_value != nullptr)
	{
		_value->UpdateTexture();

		glActiveTexture(GL_TEXTURE0 + _texture);
		CHECK_ERROR_GL();
		glBindTexture(GL_TEXTURE_2D, _value->_id);
		CHECK_ERROR_GL();
	}
	glUniform1i(_location, _texture);
	CHECK_ERROR_GL();
}


/***/


RenderCallBase::RenderCallBase(ShaderProgram* shaderprogram) :
	_shaderprogram(shaderprogram),
	_vertices(nullptr),
	_texture_count(0)
{
}


RenderCallBase::~RenderCallBase()
{
	for (RenderCallUniformBase* uniform : _uniforms)
		delete uniform;
}


void RenderCallBase::Render()
{
	if (_vertices == nullptr)
		return;

	_vertices->Update();

	if (_vertices->_vbo == 0 || _vertices->_count == 0)
		return;

	glUseProgram(_shaderprogram->_program);

	for (RenderCallUniformBase* uniform : _uniforms)
		uniform->Assign();

	for (RenderCallTexture* texture : _textures)
		texture->Assign();

	glUseProgram(_shaderprogram->_program);
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

	if (_shaderprogram->_blend_sfactor != GL_ONE || _shaderprogram->_blend_dfactor != GL_ZERO)
	{
		glEnable(GL_BLEND);
		CHECK_ERROR_GL();
		glBlendFunc(_shaderprogram->_blend_sfactor, _shaderprogram->_blend_dfactor);
		CHECK_ERROR_GL();
	}

	glDrawArrays(_vertices->_mode, 0, _vertices->_count);
	CHECK_ERROR_GL();

	if (_shaderprogram->_blend_sfactor != GL_ONE || _shaderprogram->_blend_dfactor != GL_ZERO)
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


bool RenderCallBase::HasChangedSinceLastRender() const
{
	return true;
}


RenderCallTexture* RenderCallBase::GetTexture(const char* name)
{
	RenderCallTexture* result = 0;
	GLint location = glGetUniformLocation(_shaderprogram->_program, name);
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
