#ifndef CommonShaders_H
#define CommonShaders_H

#include "ShaderProgram.h"
#include "RenderCall.h"

class GraphicsContext;


class GradientShader_2f : public ShaderProgram<Vertex_2f_4f>
{
	friend class GraphicsContext;
/*
	attribute vec3 position;
	attribute vec4 color;

	uniform mat4 transform;
	uniform float point_size;
 */
	GradientShader_2f(GraphicsContext* gc);
};


class GradientShader_3f : public ShaderProgram<Vertex_3f_4f>
{
	friend class GraphicsContext;
/*
	attribute vec3 position;
	attribute vec4 color;

	uniform mat4 transform;
	uniform float point_size;
 */
	GradientShader_3f(GraphicsContext* gc);
};


class PlainShader_2f : public ShaderProgram<Vertex_2f>
{
	friend class GraphicsContext;
/*
	attribute vec2 position;

	uniform mat4 transform;
	uniform float point_size;
	uniform vec4 color;
 */
	PlainShader_2f(GraphicsContext* gc);
};


class PlainShader_3f : public ShaderProgram<Vertex_3f>
{
	friend class GraphicsContext;
/*
	attribute vec2 position;

	uniform mat4 transform;
	uniform float point_size;
	uniform vec4 color;
 */
	PlainShader_3f(GraphicsContext* gc);
};


class TextureShader_2f : public ShaderProgram<Vertex_2f_2f>
{
	friend class GraphicsContext;
/*
	attribute vec2 position;
	attribute vec2 texcoord;

	uniform mat4 transform;
	uniform sampler2D texture;
 */
	TextureShader_2f(GraphicsContext* gc);
};


class TextureShader_3f : public ShaderProgram<Vertex_3f_2f>
{
	friend class GraphicsContext;
/*
	attribute vec3 position;
	attribute vec2 texcoord;

	uniform mat4 transform;
	uniform sampler2D texture;
 */
	TextureShader_3f(GraphicsContext* gc);
};


class OpaqueTextureShader_2f : public ShaderProgram<Vertex_2f_2f>
{
	friend class GraphicsContext;
/*
	attribute vec2 position;
	attribute vec2 texcoord;

	uniform mat4 transform;
	uniform sampler2D texture;
 */
	OpaqueTextureShader_2f(GraphicsContext* gc);
};


class AlphaTextureShader_2f : public ShaderProgram<Vertex_2f_2f>
{
	friend class GraphicsContext;
/*
	attribute vec2 position;
	attribute vec2 texcoord;

	uniform mat4 transform;
	uniform sampler2D texture;
	uniform float alpha;
 */
	AlphaTextureShader_2f(GraphicsContext* gc);
};


#endif
