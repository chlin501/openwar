#include "CommonShaders.h"



renderers* renderers::singleton = nullptr;



renderers::renderers()
{
	_distance_renderer = new ShaderProgram2<glm::vec2, glm::vec2>(
		"position", "texcoord",
		VERTEX_SHADER
		({
			attribute vec2 position;
			attribute vec2 texcoord;
			uniform mat4 transform;
			varying vec2 _texcoord;

			void main()
			{
				_texcoord = texcoord;

				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			varying vec2 _texcoord;

			const bool Outline = false;
			const bool Glow = false;
			const bool Shadow = true;
			const vec3 GlyphColor = vec3(1.0, 1.0, 1.0);
			const float SmoothCenter = 0.5;
			const vec2 ShadowOffset = vec2(-0.002, 0.004);
			const vec3 ShadowColor = vec3(0.0, 0.0, 0.0125);
			const float OutlineCenter = 0.5;
			const vec3 OutlineColor = vec3(1.0, 0.0, 0.0);
			const float GlowBoundary = 1.0;

			void main()
			{
				vec4 color = texture2D(texture, _texcoord);
				float distance = color.a;
				float smoothWidth = 0.03;//fwidth(distance);
				float alpha;
				vec3 rgb;

				if (Outline)
				{
					float mu = smoothstep(OutlineCenter - smoothWidth, OutlineCenter + smoothWidth, distance);
					alpha = smoothstep(SmoothCenter - smoothWidth, SmoothCenter + smoothWidth, distance);
					rgb = mix(OutlineColor, GlyphColor, mu);
				}

				if (Shadow)
				{
					float distance2 = texture2D(texture, _texcoord + ShadowOffset).a;
					float s = smoothstep(SmoothCenter - smoothWidth, SmoothCenter + smoothWidth, distance2);
					float v = smoothstep(SmoothCenter - smoothWidth, SmoothCenter + smoothWidth, distance);

					rgb = mix(ShadowColor, GlyphColor, v);
					alpha = max(s, v);

					/*if (v == 1.0)
					{
						rgb = GlyphColor;
						alpha = 1.0;
					}
					else if (s == 1.0 && v != 1.0)
					{
						rgb = GlyphColor;
						alpha = v;
					}
					else if (s == 0.0 && v == 1.0)
					{
						rgb = GlyphColor;
						alpha = 1.0;
					}
					else if (s == 0.0)
					{
						rgb = mix(GlyphColor, ShadowColor, v);
						alpha = 1.0;
					}
					else
					{
						rgb = mix(GlyphColor, ShadowColor, v);
						alpha = s;
					}*/
				}

				gl_FragColor = vec4(rgb, alpha);
			}
		})
	);
	_distance_renderer->_blend_sfactor = GL_ONE;
	_distance_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;


	_gradient_renderer = new ShaderProgram2<glm::vec2, glm::vec4>(
		"position", "color",
		VERTEX_SHADER
		({
			attribute vec3 position;
			attribute vec4 color;
			uniform mat4 transform;
			uniform float point_size;
			varying vec4 v_color;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, position.z, 1);

				gl_Position = p;
				gl_PointSize = point_size;

				v_color = color;
			}
		}),
		FRAGMENT_SHADER
		({
			varying vec4 v_color;

			void main()
			{
				gl_FragColor = v_color;
			}
		})
	);
	_gradient_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_gradient_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;


	_gradient_renderer3 = new ShaderProgram2<glm::vec3, glm::vec4>(
		"position", "color",
		VERTEX_SHADER
		({
			attribute vec3 position;
			attribute vec4 color;
			uniform mat4 transform;
			uniform float point_size;
			varying vec4 v_color;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, position.z, 1);

				gl_Position = p;
				gl_PointSize = point_size;
				v_color = color;
			}
		}),
		FRAGMENT_SHADER
		({
			varying vec4 v_color;

			void main()
			{
				gl_FragColor = v_color;
			}
		})
	);
	_gradient_renderer3->_blend_sfactor = GL_SRC_ALPHA;
	_gradient_renderer3->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;


	_ground_renderer = new ShaderProgram2<glm::vec2, glm::vec2>(
		"position", "texcoord",
		VERTEX_SHADER
		({
			attribute vec2 position;
			attribute vec2 texcoord;
			uniform mat4 transform;
			varying vec2 _texcoord;

			void main()
			{
				_texcoord = texcoord;

				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			uniform vec2 obstacle1;
			uniform vec2 obstacle2;
			uniform vec2 obstacle3;
			uniform vec2 obstacle4;
			uniform vec2 obstacle5;
			uniform vec2 obstacle6;
			varying vec2 _texcoord;

			bool is_obstacle(vec2 obstacle)
			{
				return obstacle[0] <= gl_FragCoord.x && gl_FragCoord.x <= obstacle[1];
			}

			void main()
			{
				bool obstacle = is_obstacle(obstacle1)
					|| is_obstacle(obstacle2)
					|| is_obstacle(obstacle3)
					|| is_obstacle(obstacle4)
					|| is_obstacle(obstacle5)
					|| is_obstacle(obstacle6);

				if (obstacle)
					gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
				else
					gl_FragColor = texture2D(texture, _texcoord);
			}
		})
	);
	_ground_renderer->_blend_sfactor = GL_ONE;
	_ground_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



	_plain_renderer = new ShaderProgram1<glm::vec2>(
		"position",
		VERTEX_SHADER
		({
			attribute vec2 position;
			uniform mat4 transform;
			uniform float point_size;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				gl_Position = p;
				gl_PointSize = point_size;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform vec4 color;

			void main()
			{
				gl_FragColor = color;
			}
		})
	);
	_plain_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_plain_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



	_plain_renderer3 = new ShaderProgram1<glm::vec3>(
		"position",
		VERTEX_SHADER
		({
			attribute vec3 position;
			uniform mat4 transform;
			uniform float point_size;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, position.z, 1);

				gl_Position = p;
				gl_PointSize = point_size;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform vec4 color;

			void main()
			{
				gl_FragColor = color;
			}
		})
	);
	_plain_renderer3->_blend_sfactor = GL_SRC_ALPHA;
	_plain_renderer3->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



	_texture_renderer = new ShaderProgram2<glm::vec2, glm::vec2>(
		"position", "texcoord",
		VERTEX_SHADER
		({
			uniform mat4 transform;
			attribute vec2 position;
			attribute vec2 texcoord;
			varying vec2 _texcoord;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				_texcoord = texcoord;

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			varying vec2 _texcoord;

			void main()
			{
				gl_FragColor = texture2D(texture, _texcoord);
			}
		})
	);
	_texture_renderer->_blend_sfactor = GL_ONE;
	_texture_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



	_texture_renderer3 = new ShaderProgram2<glm::vec3, glm::vec2>(
		"position", "texcoord",
		VERTEX_SHADER
		({
			uniform mat4 transform;
			attribute vec3 position;
			attribute vec2 texcoord;
			varying vec2 _texcoord;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, position.z, 1);

				_texcoord = texcoord;

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			varying vec2 _texcoord;

			void main()
			{
				gl_FragColor = texture2D(texture, _texcoord);
			}
		})
	);
	_texture_renderer3->_blend_sfactor = GL_ONE;
	_texture_renderer3->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;



	_opaque_texture_renderer = new ShaderProgram2<glm::vec2, glm::vec2>(
		"position", "texcoord",
		VERTEX_SHADER
		({
			uniform mat4 transform;
			attribute vec2 position;
			attribute vec2 texcoord;
			varying vec2 _texcoord;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				_texcoord = texcoord;

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			varying vec2 _texcoord;

			void main()
			{
				gl_FragColor = texture2D(texture, _texcoord);
			}
		})
	);
	_opaque_texture_renderer->_blend_sfactor = GL_ONE;
	_opaque_texture_renderer->_blend_dfactor = GL_ZERO;


	_alpha_texture_renderer = new ShaderProgram2<glm::vec2, glm::vec2>(
		"position", "texcoord",
		VERTEX_SHADER
		({
			uniform mat4 transform;
			attribute vec2 position;
			attribute vec2 texcoord;
			varying vec2 _texcoord;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				_texcoord = texcoord;

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			uniform float alpha;
			varying vec2 _texcoord;

			void main()
			{
				vec4 c = texture2D(texture, _texcoord) * alpha;
				gl_FragColor = c;
			}
		})
	);
	_alpha_texture_renderer->_blend_sfactor = GL_ONE;
	_alpha_texture_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;

}
