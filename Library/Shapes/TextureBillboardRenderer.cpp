// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <algorithm>
#include "TextureBillboardRenderer.h"


TextureBillboardRenderer::TextureBillboardRenderer(GraphicsContext* gc)
{
	static int shaderprogram_id = GraphicsContext::generate_shaderprogram_id();

	_shaderprogram = gc->LoadShaderProgram4<glm::vec3, float, glm::vec2, glm::vec2>(
		shaderprogram_id,
		"position", "height", "texcoord", "texsize",
		VERTEX_SHADER
		({
			uniform
			mat4 transform;
			uniform
			vec3 upvector;
			uniform float viewport_height;
			uniform float min_point_size;
			uniform float max_point_size;
			attribute
			vec3 position;
			attribute float height;
			attribute
			vec2 texcoord;
			attribute
			vec2 texsize;
			varying
			vec2 _texcoord;
			varying
			vec2 _texsize;

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
			uniform
			sampler2D texture;
			varying
			vec2 _texcoord;
			varying
			vec2 _texsize;

			void main()
			{
				vec4 color = texture2D(texture, _texcoord + gl_PointCoord * _texsize);

				gl_FragColor = color;
			}
		}));
	_shaderprogram->_blend_sfactor = GL_ONE;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


TextureBillboardRenderer::~TextureBillboardRenderer()
{
}


void TextureBillboardRenderer::Reset()
{
	_vertices._mode = GL_POINTS;
	_vertices._vertices.clear();
}


void TextureBillboardRenderer::AddBillboard(glm::vec3 position, float height, affine2 texcoords)
{
	glm::vec2 texpos = texcoords.transform(glm::vec2(0, 0));
	glm::vec2 texsize = texcoords.transform(glm::vec2(1, 1)) - texpos;

	_vertices._vertices.push_back(Vertex_3f_1f_2f_2f(position, height, texpos, texsize));
}



struct billboard_index
{
	int index;
	float order;
};

void TextureBillboardRenderer::Draw(texture* tex, const glm::mat4x4& transform, const glm::vec3& cameraUp, float cameraFacingDegrees, float viewportHeight, bounds1f sizeLimit)
{
	static std::vector<Vertex_3f_1f_2f_2f> vertices;
	static std::vector<billboard_index> indices;

	vertices.insert(vertices.end(), _vertices._vertices.begin(), _vertices._vertices.end());

	float a = -glm::radians(cameraFacingDegrees);
	float cos_a = cosf(a);
	float sin_a = sinf(a);

	int index = 0;
	for (Vertex_3f_1f_2f_2f& v : _vertices._vertices)
	{
		billboard_index i;
		i.index = index++;
		i.order = cos_a * v._1.x - sin_a * v._1.y;
		indices.push_back(i);
	}

	std::sort(indices.begin(), indices.end(), [](const billboard_index& a, const billboard_index& b) -> bool {
		float diff = a.order - b.order;
		return diff == 0 ? a.index < b.index : diff > 0;
	});

	_vertices._vertices.clear();
	for (const billboard_index& i : indices)
		_vertices._vertices.push_back(vertices[i.index]);

	vertices.clear();
	indices.clear();

	_vertices.UpdateVBO(GL_STATIC_DRAW);

	_shaderprogram->get_uniform<glm::mat4>("transform").set_value(transform);
	_shaderprogram->get_uniform<const texture*>("texture").set_value(tex);
	_shaderprogram->get_uniform<glm::vec3>("upvector").set_value(cameraUp);
	_shaderprogram->get_uniform<float>("viewport_height").set_value(ShaderProgramBase::pixels_per_point() * viewportHeight);
	_shaderprogram->get_uniform<float>("min_point_size").set_value(sizeLimit.min);
	_shaderprogram->get_uniform<float>("max_point_size").set_value(sizeLimit.max);
	_shaderprogram->render(_vertices);
}


static affine2 FlipY(const affine2& texcoords)
{
	glm::vec2 v0 = texcoords.transform(glm::vec2(0, 0));
	glm::vec2 v1 = texcoords.transform(glm::vec2(1, 1));
	return affine2(glm::vec2(v0.x, v1.y), glm::vec2(v1.x, v0.y));
}


void TextureBillboardRenderer::Render(BillboardModel* billboardModel, glm::mat4x4 const & transform, const glm::vec3& cameraUp, float cameraFacingDegrees, float viewportHeight, bool flip)
{
	Reset();

	for (const Billboard& billboard : billboardModel->staticBillboards)
	{
		float facing = billboard.facing - cameraFacingDegrees + 180;
		affine2 texcoords = billboardModel->texture->GetTexCoords(billboard.shape, flip ? -facing : facing);
		if (flip)
			texcoords = FlipY(texcoords);
		AddBillboard(billboard.position, billboard.height, texcoords);
	}

	for (const Billboard& billboard : billboardModel->dynamicBillboards)
	{
		float facing = billboard.facing - cameraFacingDegrees + 180;
		affine2 texcoords = billboardModel->texture->GetTexCoords(billboard.shape, flip ? -facing : facing);
		if (flip)
			texcoords = FlipY(texcoords);
		AddBillboard(billboard.position, billboard.height, texcoords);
	}

	Draw(billboardModel->texture->GetTexture(), transform, cameraUp, cameraFacingDegrees, viewportHeight);
}
