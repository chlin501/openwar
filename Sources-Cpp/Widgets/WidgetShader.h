#ifndef WidgetShader_H
#define WidgetShader_H

#include "Graphics/ShaderProgram.h"


class WidgetShader : public ShaderProgram
{
	friend class GraphicsContext;
	/*
		attribute vec2 position;
		attribute vec2 texcoord;
		attribute vec4 colorize;
		attribute float alpha;

		uniform mat4 transform;
		uniform sampler2D texture;
	 */
	WidgetShader(GraphicsContext* gc);
};


#endif
