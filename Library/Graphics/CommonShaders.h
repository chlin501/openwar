#ifndef CommonShaders_H
#define CommonShaders_H

#include "ShaderProgram.h"

class GraphicsContext;


class GradientShader2 : public ShaderProgram2<glm::vec2, glm::vec4>
{
	friend class GraphicsContext;
	GradientShader2(GraphicsContext* gc);
};


class PlainShader2 : public ShaderProgram1<glm::vec2>
{
	friend class GraphicsContext;
	PlainShader2(GraphicsContext* gc);
};


class PlainShader3 : public ShaderProgram1<glm::vec3>
{
	friend class GraphicsContext;
	PlainShader3(GraphicsContext* gc);
};


class TextureShader2 : public ShaderProgram2<glm::vec2, glm::vec2>
{
	friend class GraphicsContext;
	TextureShader2(GraphicsContext* gc);
};


class TextureShader3 : public ShaderProgram2<glm::vec3, glm::vec2>
{
	friend class GraphicsContext;
	TextureShader3(GraphicsContext* gc);
};


class OpaqueTextureShader2 : public ShaderProgram2<glm::vec2, glm::vec2>
{
	friend class GraphicsContext;
	OpaqueTextureShader2(GraphicsContext* gc);
};


class AlphaTextureShader2 : public ShaderProgram2<glm::vec2, glm::vec2>
{
	friend class GraphicsContext;
	AlphaTextureShader2(GraphicsContext* gc);
};


#endif
