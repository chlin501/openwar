// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif
#endif

#include "renderer.h"


#ifndef CHECK_ERROR_GL
void CHECK_ERROR_GL()
{
}
#endif


renderers* renderers::singleton = nullptr;


static void print_log(const char* operation, const char* log)
{
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
	NSLog(@"RENDERER log (%s):\n%s", operation, log);
#endif
}


renderer_specification operator,(renderer_vertex_attribute x, renderer_vertex_attribute y)
{
	renderer_specification result;
	result._vertex_attributes.push_back(x);
	result._vertex_attributes.push_back(y);
	return result;
}


renderer_specification operator,(renderer_specification x, renderer_vertex_attribute y)
{
	renderer_specification result;
	result._vertex_attributes = x._vertex_attributes;
	result._vertex_attributes.push_back(y);
	return result;
}





static const GLchar* decode_name(const GLchar* name)
{
	return *name == '_' ? name + 1 : name;
}


renderer_base::renderer_base(const renderer_specification& specification, const char* vertexshader, const char* fragmentshader) :
_nextUniformTexture(0),
_vertex_attributes(specification._vertex_attributes),
_blend_sfactor(GL_ONE),
_blend_dfactor(GL_ZERO)
{
	_program = glCreateProgram();
	CHECK_ERROR_GL();

	GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertexshader);
	GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragmentshader);

    glAttachShader(_program, vertex_shader);
	CHECK_ERROR_GL();
    glAttachShader(_program, fragment_shader);
	CHECK_ERROR_GL();

	for (GLuint index = 0; index < _vertex_attributes.size(); ++index)
	{
		const GLchar* name = decode_name(_vertex_attributes[index]._name);
		glBindAttribLocation(_program, index, name);
		CHECK_ERROR_GL();
	}

    if (!link_program(_program)) {
        if (_program) {
            glDeleteProgram(_program);
	        CHECK_ERROR_GL();
	        _program = 0;
        }
        return;
    }
	validate_program(_program);

	glDetachShader(_program, vertex_shader);
	CHECK_ERROR_GL();
	glDetachShader(_program, fragment_shader);
	CHECK_ERROR_GL();

	glDeleteShader(vertex_shader);
	CHECK_ERROR_GL();
	glDeleteShader(fragment_shader);
	CHECK_ERROR_GL();
}



renderer_base::~renderer_base()
{
    if (_program != 0)
    {
        glDeleteProgram(_program);
	    CHECK_ERROR_GL();
    }
}



float renderer_base::pixels_per_point()
{
	static float result = 0;
	if (result == 0)
	{
#if TARGET_OS_IPHONE
		if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)])
			result = [[UIScreen mainScreen] scale];
		else
			result = 1;
#else
		result = 1;
#endif
	}

	return result;
}



GLuint renderer_base::compile_shader(GLenum type, const char* source)
{
    std::string str(source);
    
    if (str.size() >= 2 && str[0] == '{' && str[str.size() - 1] == '}')
        str = str.substr(1, str.size() - 2);
    
#if TARGET_OS_IPHONE
    str.insert(0, "precision highp float; precision lowp int; ");
#else
    str.insert(0, "#version 120\n");
#endif

	const GLchar *src = str.c_str();

	GLuint result = glCreateShader(type);
	CHECK_ERROR_GL();
	glShaderSource(result, 1, &src, NULL);
	CHECK_ERROR_GL();
	glCompileShader(result);
	CHECK_ERROR_GL();

	#if 1 //defined(DEBUG)
	GLint logLength;
	glGetShaderiv(result, GL_INFO_LOG_LENGTH, &logLength);
	CHECK_ERROR_GL();
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)malloc((size_t)logLength);
		glGetShaderInfoLog(result, logLength, &logLength, log);
		CHECK_ERROR_GL();
		print_log("compile", log);
		free(log);
	}
	#endif

	//GLint status;
	//glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

	return result;
}



bool renderer_base::link_program(GLuint program)
{
    GLint status;
    glLinkProgram(program);
	CHECK_ERROR_GL();

#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	CHECK_ERROR_GL();
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc((size_t)logLength);
        glGetProgramInfoLog(program, logLength, &logLength, log);
	    CHECK_ERROR_GL();
		print_log("log", log);
        free(log);
    }
#endif

    glGetProgramiv(program, GL_LINK_STATUS, &status);
	CHECK_ERROR_GL();
    if (status == 0) {
        return false;
    }

    return true;
}


bool renderer_base::validate_program(GLuint program)
{
	GLint logLength, status;

	glValidateProgram(program);
	CHECK_ERROR_GL();
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	CHECK_ERROR_GL();
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)malloc((size_t)logLength);
		glGetProgramInfoLog(program, logLength, &logLength, log);
		print_log("validate", log);
		free(log);
	}

	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	CHECK_ERROR_GL();
	if (status == 0) {
		return false;
	}

	return true;
}



renderers::renderers()
{
	_distance_renderer = new renderer<texture_vertex>((
		VERTEX_ATTRIBUTE(texture_vertex, _position),
		VERTEX_ATTRIBUTE(texture_vertex, _texcoord)),
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


	_gradient_renderer = new renderer<color_vertex>((
		VERTEX_ATTRIBUTE(color_vertex, _position),
		VERTEX_ATTRIBUTE(color_vertex, _color)),
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


	_gradient_renderer3 = new renderer<color_vertex3>((
		VERTEX_ATTRIBUTE(color_vertex3, _position),
		VERTEX_ATTRIBUTE(color_vertex3, _color)),
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


	_ground_renderer = new renderer<texture_vertex>((
		VERTEX_ATTRIBUTE(texture_vertex, _position),
		VERTEX_ATTRIBUTE(texture_vertex, _texcoord)),
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



	_plain_renderer = new renderer<plain_vertex>((
		VERTEX_ATTRIBUTE(plain_vertex, _position)),
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



	_plain_renderer3 = new renderer<plain_vertex3>((
		VERTEX_ATTRIBUTE(plain_vertex3, _position)),
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



	_texture_renderer = new renderer<texture_vertex>((
		VERTEX_ATTRIBUTE(texture_vertex, _position),
		VERTEX_ATTRIBUTE(texture_vertex, _texcoord)),
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



	_texture_renderer3 = new renderer<texture_vertex3>((
			VERTEX_ATTRIBUTE(texture_vertex3, _position),
					VERTEX_ATTRIBUTE(texture_vertex3, _texcoord)),
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



	_opaque_texture_renderer = new renderer<texture_vertex>((
		VERTEX_ATTRIBUTE(texture_vertex, _position),
		VERTEX_ATTRIBUTE(texture_vertex, _texcoord)),
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


	_alpha_texture_renderer = new renderer<texture_vertex>((
			VERTEX_ATTRIBUTE(texture_vertex, _position),
			VERTEX_ATTRIBUTE(texture_vertex, _texcoord)),
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
