// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ColorBillboardShape.h"



ColorBillboardShader::ColorBillboardShader(GraphicsContext* gc) : ShaderProgram3<glm::vec3, glm::vec4, float>(
	"position", "color", "height",
	VERTEX_SHADER
	({
		uniform
		mat4 transform;
		uniform
		vec3 upvector;
		uniform float viewport_height;
		attribute
		vec3 position;
		attribute
		vec4 color;
		attribute float height;
		varying
		vec4 _color;

		void main()
		{
			float scale = 0.5 * height * viewport_height;
			vec3 position2 = position + scale * upvector;
			vec4 p = transform * vec4(position, 1);
			vec4 q = transform * vec4(position2, 1);
			float s = abs(q.y / q.w - p.y / p.w);

			float a = color.a;
			if (s < 1.0)
			{
				a = a * s;
				s = 1.0;
			}

			_color = vec4(color.rgb, a);
			gl_Position = p;
			gl_PointSize = s;
		}
	}),
	FRAGMENT_SHADER
	({
		varying
		vec4 _color;

		void main()
		{
			gl_FragColor = _color;
		}
	}))
{
	_blend_sfactor = GL_SRC_ALPHA;
	_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


void ColorBillboardShader::SetTransform(const glm::mat4x4& value)
{
	get_uniform<glm::mat4>("transform").set_value(value);
}


void ColorBillboardShader::SetCameraUp(const glm::vec3 value)
{
	get_uniform<glm::vec3>("upvector").set_value(value);
}


void ColorBillboardShader::SetViewportHeight(float value)
{
	float viewport_height = 0.25f * ShaderProgramBase::pixels_per_point() * value;
	get_uniform<float>("viewport_height").set_value(viewport_height);
}


void ColorBillboardShader::Render(VertexBuffer_3f_4f_1f* vertices)
{
	render(*vertices);
}


ColorBillboardShape::ColorBillboardShape(GraphicsContext* gc)
{
	_shader = new ColorBillboardShader(gc);
}


ColorBillboardShape::~ColorBillboardShape()
{
	delete _shader;
}


void ColorBillboardShape::Reset()
{
	_vertices._mode = GL_POINTS;
	_vertices.Clear();
}


void ColorBillboardShape::AddBillboard(const glm::vec3& position, const glm::vec4& color, float height)
{
	_vertices.AddVertex(Vertex_3f_4f_1f(position, color, height));
}


void ColorBillboardShape::Draw(const glm::mat4x4& transform, const glm::vec3 cameraUp, float viewportHeight)
{
	_shader->SetTransform(transform);
	_shader->SetCameraUp(cameraUp);
	_shader->SetViewportHeight(viewportHeight);
	_shader->Render(&_vertices);
}
