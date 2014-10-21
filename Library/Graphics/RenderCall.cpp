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


void RenderCallBase::PreRender()
{
	glUseProgram(_shaderprogram->_program);
	for (UniformBase* uniform : _uniforms)
		uniform->Assign();
}
