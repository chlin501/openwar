#ifndef CommonShaders_H
#define CommonShaders_H

#include "ShaderProgram.h"

class GraphicsContext;


class GradientShader2
{
public:
	ShaderProgram2<glm::vec2, glm::vec4>* _shaderprogram;
public:
	GradientShader2(GraphicsContext* gc);
};


class PlainShader2
{
public:
	ShaderProgram1<glm::vec2>* _shaderprogram;
public:
	PlainShader2(GraphicsContext* gc);
};


class PlainShader3
{
public:
	ShaderProgram1<glm::vec3>* _shaderprogram;
public:
	PlainShader3(GraphicsContext* gc);
};


class TextureShader2
{
public:
	ShaderProgram2<glm::vec2, glm::vec2>* _shaderprogram;
public:
	TextureShader2(GraphicsContext* gc);
};


class TextureShader3
{
public:
	ShaderProgram2<glm::vec3, glm::vec2>* _shaderprogram;
public:
	TextureShader3(GraphicsContext* gc);
};


class OpaqueTextureShader2
{
public:
	ShaderProgram2<glm::vec2, glm::vec2>* _shaderprogram;
public:
	OpaqueTextureShader2(GraphicsContext* gc);
};


class AlphaTextureShader2
{
public:
	ShaderProgram2<glm::vec2, glm::vec2>* _shaderprogram;
public:
	AlphaTextureShader2(GraphicsContext* gc);
};



struct renderers
{
	static renderers* singleton;

	GradientShader2* _gradient_renderer;
	PlainShader2* _plain_renderer;
	PlainShader3* _plain_renderer3;
	TextureShader2* _texture_renderer;
	TextureShader3* _texture_renderer3;
	OpaqueTextureShader2* _opaque_texture_renderer;
	AlphaTextureShader2* _alpha_texture_renderer;

	renderers();
};


#endif
