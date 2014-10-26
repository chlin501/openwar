#include "RenderCall.h"



UniformBase::UniformBase(GLint location) :
	_location(location),
	_texture(0)
{
}


UniformBase::~UniformBase()
{
}


void UniformBase::Assign(int value)
{
	glUniform1iv(_location, 1, (const GLint*)&value);
	CHECK_ERROR_GL();
}


void UniformBase::Assign(float value)
{
	glUniform1fv(_location, 1, (const GLfloat*)&value);
	CHECK_ERROR_GL();
}


void UniformBase::Assign(const glm::vec2& value)
{
	glUniform2fv(_location, 1, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


void UniformBase::Assign(const glm::vec3& value)
{
	glUniform3fv(_location, 1, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


void UniformBase::Assign(const glm::vec4& value)
{
	glUniform4fv(_location, 1, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


void UniformBase::Assign(const glm::mat2& value)
{
	glUniformMatrix2fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


void UniformBase::Assign(const glm::mat3& value)
{
	glUniformMatrix3fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


void UniformBase::Assign(const glm::mat4& value)
{
	glUniformMatrix4fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&value));
	CHECK_ERROR_GL();
}


void UniformBase::Assign(const texture* value)
{
	if (value != nullptr)
	{
		glActiveTexture(GL_TEXTURE0 + _texture);
		CHECK_ERROR_GL();
		glBindTexture(GL_TEXTURE_2D, value->id);
		CHECK_ERROR_GL();
	}
	glUniform1i(_location, _texture);
	CHECK_ERROR_GL();
}


RenderCallBase::RenderCallBase(ShaderProgramBase* shaderprogram) :
	_shaderprogram(shaderprogram),
	_textures(0)
{
}


RenderCallBase::~RenderCallBase()
{
	for (UniformBase* uniform : _uniforms)
		delete uniform;
}


void RenderCallBase::Render()
{
	VertexBufferBase* vertices = GetVertexBufferBase();
	if (vertices == nullptr)
		return;

	vertices->Update();

	if (vertices->_vbo == 0)
		return;

	glUseProgram(_shaderprogram->_program);
	for (UniformBase* uniform : _uniforms)
		uniform->Assign();

	glUseProgram(_shaderprogram->_program);
	CHECK_ERROR_GL();

	if (vertices->_vbo != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertices->_vbo);
		CHECK_ERROR_GL();
	}

	const char* ptr = nullptr;//vertices->_vbo != 0 ? nullptr : reinterpret_cast<const char*>(vertices->data());
	for (GLuint index = 0; index < _shaderprogram->_vertex_attributes.size(); ++index)
	{
		glEnableVertexAttribArray(index);
		CHECK_ERROR_GL();

		const renderer_vertex_attribute& item = _shaderprogram->_vertex_attributes[index];
		const GLvoid* offset = reinterpret_cast<const GLvoid*>(ptr + item._offset);

		glVertexAttribPointer(index, item._size, item._type, GL_FALSE, item._stride, offset);
		CHECK_ERROR_GL();
	}

	if (_shaderprogram->_blend_sfactor != GL_ONE || _shaderprogram->_blend_dfactor != GL_ZERO)
	{
		glEnable(GL_BLEND);
		CHECK_ERROR_GL();
		glBlendFunc(_shaderprogram->_blend_sfactor, _shaderprogram->_blend_dfactor);
		CHECK_ERROR_GL();
	}

	glDrawArrays(vertices->_mode, 0, vertices->_count);
	CHECK_ERROR_GL();

	if (_shaderprogram->_blend_sfactor != GL_ONE || _shaderprogram->_blend_dfactor != GL_ZERO)
	{
		glDisable(GL_BLEND);
		CHECK_ERROR_GL();
		glBlendFunc(GL_ONE, GL_ZERO);
		CHECK_ERROR_GL();
	}

	for (GLuint index = 0; index < _shaderprogram->_vertex_attributes.size(); ++index)
	{
		glDisableVertexAttribArray(index);
		CHECK_ERROR_GL();
	}

	if (vertices->_vbo != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_ERROR_GL();
	}
}
