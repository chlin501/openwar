#include "WidgetShader.h"


WidgetShader::WidgetShader(GraphicsContext* gc) : ShaderProgram(
	VERTEX_SHADER
	({
		attribute vec2 position;
		attribute vec2 texcoord;
		attribute vec4 colorize;
		attribute float alpha;
		uniform mat4 transform;
		varying vec2 _texcoord;
		varying vec4 _colorize;
		varying float _alpha;

		void main()
		{
			vec4 p = transform * vec4(position.x, position.y, 0, 1);

			_texcoord = texcoord;
			_colorize = colorize;
			_alpha = alpha;

			gl_Position = vec4(p.x, p.y, 0.5, 1.0);
		}
	}),
	FRAGMENT_SHADER
	({
		uniform sampler2D texture;
		varying vec2 _texcoord;
		varying vec4 _colorize;
		varying float _alpha;

		void main()
		{
			vec4 color = texture2D(texture, _texcoord);
			color.rgb = mix(color.rgb, _colorize.rgb * color.a, _colorize.a);
			color = color * clamp(_alpha, 0.0, 1.0);

			gl_FragColor = color;
		}
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}
