#include "BillboardTextureShader.h"


BillboardTextureShader::BillboardTextureShader(GraphicsContext* gc) : ShaderProgram(
	VERTEX_SHADER
	({
		uniform mat4 transform;
		uniform vec3 upvector;
		uniform float viewport_height;
		uniform float min_point_size;
		uniform float max_point_size;
		attribute vec3 position;
		attribute float height;
		attribute vec2 texcoord;
		attribute vec2 texsize;
		varying vec2 _texcoord;
		varying vec2 _texsize;

		void main()
		{
			vec3 position2 = position + height * 0.5 * viewport_height * upvector;
			vec4 p = transform * vec4(position, 1);
			vec4 q = transform * vec4(position2, 1);
			float s = clamp(abs(q.y / q.w - p.y / p.w), min_point_size, max_point_size);

			_texcoord = texcoord;
			_texsize = texsize;

			gl_Position = p;
			gl_PointSize = s;
		}
	}),
	FRAGMENT_SHADER
	({
		uniform sampler2D texture;
		varying vec2 _texcoord;
		varying vec2 _texsize;

		void main()
		{
			vec4 color = texture2D(texture, _texcoord + gl_PointCoord * _texsize);

			gl_FragColor = color;
		}
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}

