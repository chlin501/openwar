// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SmoothTerrainShaders.h"
#include "Graphics/Image.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/RenderCall.h"
#include "Graphics/TextureAtlas.h"


TerrainInsideShader::TerrainInsideShader(GraphicsContext* gc) : ShaderProgram(
	VERTEX_SHADER
	({
		uniform mat4 transform;
		uniform vec4 map_bounds;
		uniform vec3 light_normal;

		attribute vec3 position;
		attribute vec3 normal;

		varying vec3 _position;
		varying vec2 _colorcoord;
		varying vec2 _splatcoord;
		varying float _brightness;

		void main()
		{
			vec4 p = transform * vec4(position, 1);

			float brightness = -dot(light_normal, normal);

			_position = position;
			_colorcoord = vec2(brightness, 1.0 - (2.5 + position.z) / 128.0);
			_splatcoord = (position.xy - map_bounds.xy) / map_bounds.zw;
			_brightness = brightness;


			gl_Position = p;
			gl_PointSize = 1.0;
		}
	}),
	FRAGMENT_SHADER
	({
		uniform sampler2D colormap;
		uniform sampler2D splatmap;

		varying vec3 _position;
		varying vec2 _colorcoord;
		varying vec2 _splatcoord;
		varying float _brightness;

		void main()
		{
			vec3 color = texture2D(colormap, _colorcoord).rgb;
			vec3 splat = texture2D(splatmap, _splatcoord).rgb;

			color = mix(color, vec3(0.45), 0.4 * step(0.5, splat.r));
			float f = step(0.0, _position.z) * smoothstep(0.475, 0.525, splat.g);
			color = mix(color, vec3(0.2196, 0.3608, 0.1922), 0.25 * f);
			color = mix(color, vec3(0), 0.03 * step(0.5, 1.0 - _brightness));

			gl_FragColor = vec4(color, 1.0);
		}
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ZERO;
}


TerrainBorderShader::TerrainBorderShader(GraphicsContext* gc) : ShaderProgram(
	VERTEX_SHADER
	({
		uniform mat4 transform;
		uniform vec4 map_bounds;
		uniform vec3 light_normal;

		attribute vec3 position;
		attribute vec3 normal;

		varying vec3 _position;
		varying vec2 _colorcoord;
		varying vec2 _splatcoord;
		varying float _brightness;

		void main()
		{
			vec4 p = transform * vec4(position, 1);

			float brightness = -dot(light_normal, normal);

			_position = position;
			_colorcoord = vec2(brightness, 1.0 - (2.5 + position.z) / 128.0);
			_splatcoord = (position.xy - map_bounds.xy) / map_bounds.zw;
			_brightness = brightness;

			gl_Position = p;
			gl_PointSize = 1.0;
		}
	}),
	FRAGMENT_SHADER
	({
		uniform sampler2D colormap;
		uniform sampler2D splatmap;

		varying vec3 _position;
		varying vec2 _colorcoord;
		varying vec2 _splatcoord;
		varying float _brightness;

		void main()
		{
			if (distance(_splatcoord, vec2(0.5, 0.5)) > 0.5)
				discard;

			vec3 color = texture2D(colormap, _colorcoord).rgb;
			vec3 splat = texture2D(splatmap, _splatcoord).rgb;

			color = mix(color, vec3(0.45), 0.4 * step(0.5, splat.r));
			float f = step(0.0, _position.z) * smoothstep(0.475, 0.525, splat.g);
			color = mix(color, vec3(0.2196, 0.3608, 0.1922), 0.3 * f);
			color = mix(color, vec3(0), 0.03 * step(0.5, 1.0 - _brightness));

			gl_FragColor = vec4(color, 1.0);
		}
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ZERO;
}


TerrainSkirtShader::TerrainSkirtShader(GraphicsContext* gc) : ShaderProgram(
	VERTEX_SHADER
	({
		attribute vec3 position;
		attribute float height;

		uniform mat4 transform;

		varying vec2 _colorcoord;
		varying float _height;

		void main()
		{
			vec4 p = transform * vec4(position, 1);

			_colorcoord = vec2(0.2, 1.0 - (2.5 + height) / 128.0);
			_height = 0.85 * (position.z + 2.5) / (height + 2.5);

			gl_Position = p;
		}
	}),
	FRAGMENT_SHADER
	({
		uniform sampler2D texture;

		varying vec2 _colorcoord;
		varying float _height;

		void main()
		{
			vec3 color = texture2D(texture, _colorcoord).rgb;
			color = mix(vec3(0.15), color, _height);

			gl_FragColor = vec4(color, 1);
		}
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ZERO;
}


DepthInsideShader::DepthInsideShader(GraphicsContext* gc) : ShaderProgram(
	VERTEX_SHADER
	({
		uniform mat4 transform;

		attribute vec3 position;
		attribute vec3 normal;

		void main()
		{
			vec4 p = transform * vec4(position, 1);
			gl_Position = p;
		}
	}),
	FRAGMENT_SHADER
	({
		void main()
		{
			gl_FragColor = vec4(1, 1, 1, 1);
		}
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ZERO;
}


DepthBorderShader::DepthBorderShader(GraphicsContext* gc) : ShaderProgram(
	VERTEX_SHADER
	({
		uniform mat4 transform;
		uniform vec4 map_bounds;

		attribute vec3 position;
		attribute vec3 normal;

		varying vec2 _terraincoord;

		void main()
		{
			_terraincoord = (position.xy - map_bounds.xy) / map_bounds.zw;
			vec4 p = transform * vec4(position, 1);
			gl_Position = p;
		}
	}),
	FRAGMENT_SHADER
	({
		varying vec2 _terraincoord;

		void main()
		{
			if (distance(_terraincoord, vec2(0.5, 0.5)) > 0.5)
				discard;

			gl_FragColor = vec4(1);
		}
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ZERO;
}


DepthSkirtShader::DepthSkirtShader(GraphicsContext* gc) : ShaderProgram(
	VERTEX_SHADER
	({
		uniform mat4 transform;

		attribute vec3 position;
		attribute float height;

		void main()
		{
			vec4 p = transform * vec4(position, 1);

			gl_Position = p;
		}
	}),
	FRAGMENT_SHADER
	({
		void main()
		{
			gl_FragColor = vec4(1);
		}
	}))
{
	_blend_sfactor = GL_ONE;
	_blend_dfactor = GL_ZERO;
}


SobelFilterShader::SobelFilterShader(GraphicsContext* gc) : ShaderProgram(
	VERTEX_SHADER
	({
		uniform mat4 transform;

		attribute vec2 position;
		attribute vec2 texcoord;

		varying vec2 coord11;
		//varying vec2 coord12;
		varying vec2 coord13;
		//varying vec2 coord21;
		//varying vec2 coord23;
		varying vec2 coord31;
		//varying vec2 coord32;
		varying vec2 coord33;

		void main()
		{
			const float dx = 1.0 / 2.0 / 1024.0;
			const float dy = 1.0 / 2.0 / 768.0;

			vec4 p = transform * vec4(position, 0, 1);

			gl_Position = p;

			coord11 = texcoord + vec2(-dx,  dy);
			//coord12 = texcoord + vec2(0.0,  dy);
			coord13 = texcoord + vec2( dx,  dy);
			//coord21 = texcoord + vec2(-dx, 0.0);
			//coord23 = texcoord + vec2( dx, 0.0);
			coord31 = texcoord + vec2(-dx, -dy);
			//coord32 = texcoord + vec2(0.0, -dy);
			coord33 = texcoord + vec2( dx, -dy);
		}
	}),
	FRAGMENT_SHADER
	({
		uniform sampler2D depth;

		varying vec2 coord11;
		//varying vec2 coord12;
		varying vec2 coord13;
		//varying vec2 coord21;
		//varying vec2 coord23;
		varying vec2 coord31;
		//varying vec2 coord32;
		varying vec2 coord33;

		void main()
		{
			float value11 = texture2D(depth, coord11).r;
			//float value12 = texture2D(depth, coord12).r;
			float value13 = texture2D(depth, coord13).r;
			//float value21 = texture2D(depth, coord21).r;
			//float value23 = texture2D(depth, coord23).r;
			float value31 = texture2D(depth, coord31).r;
			//float value32 = texture2D(depth, coord32).r;
			float value33 = texture2D(depth, coord33).r;

			float h = value11 - value33;
			float v = value31 - value13;

			//float h = -value11 - 2.0 * value12 - value13 + value31 + 2.0 * value32 + value33;
			//float v = -value31 - 2.0 * value21 - value11 + value33 + 2.0 * value23 + value13;

			float k = clamp(5.0 * length(vec2(h, v)), 0.0, 0.6);

			//gl_FragColor = vec4(0.145, 0.302, 0.255, k);
			gl_FragColor = vec4(0.0725, 0.151, 0.1275, k);
		}
	}))
{
	_blend_sfactor = GL_SRC_ALPHA;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


GroundShadowShader::GroundShadowShader(GraphicsContext* gc) : ShaderProgram(
	VERTEX_SHADER
	({
		uniform mat4 transform;
		uniform vec4 map_bounds;

		attribute vec2 position;

		varying vec2 _groundpos;

		void main()
		{
			vec4 p = transform * vec4(position, -2.5, 1);

			_groundpos = (position - map_bounds.xy) / map_bounds.zw;

			gl_Position = p;
			gl_PointSize = 1.0;
		}
	}),
	FRAGMENT_SHADER
	({
		varying vec2 _groundpos;

		void main()
		{
			float d = distance(_groundpos, vec2(0.5, 0.5)) - 0.5;
			float a = clamp(0.3 - d * 24.0, 0.0, 0.3);

			gl_FragColor = vec4(0, 0, 0, a);
		}
	}))
{
	_blend_sfactor = GL_SRC_ALPHA;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}
