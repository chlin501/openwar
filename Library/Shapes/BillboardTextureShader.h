#ifndef BillboardTextureShader_H
#define BillboardTextureShader_H

#include "ShaderProgram.h"


class BillboardTextureShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec3 position;
		attribute float height;
		attribute vec2 texcoord;
		attribute vec2 texsize;

		uniform mat4 transform;
		uniform vec3 upvector;
		uniform float viewport_height;
		uniform float min_point_size;
		uniform float max_point_size;
	 */
	BillboardTextureShader(GraphicsContext* gc);
};


#endif
