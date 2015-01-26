// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SmoothTerrainRenderer.h"
#include "Graphics/Image.h"
#include "Graphics/CommonShaders.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/TextureAtlas.h"


SmoothTerrainRenderer::SmoothTerrainRenderer(GraphicsContext* gc, SmoothGroundMap* smoothGroundMap) :
_gc(gc),
_smoothGroundMap(smoothGroundMap),
_framebuffer_width(0),
_framebuffer_height(0),
_framebuffer(nullptr),
_colorbuffer(nullptr),
_depth(nullptr),
_colormap(nullptr),
_splatmap(nullptr),
_showLines(false),
_editMode(false)
{
	_renderers = new SmoothTerrainShaders();
	_colormap = SmoothTerrainShaders::create_colormap(gc);

	_splatmap = new TextureAtlas(gc);
	UpdateSplatmap();

	InitializeSkirt();
	InitializeShadow();
	InitializeLines();

	BuildTriangles();
}


SmoothTerrainRenderer::~SmoothTerrainRenderer()
{
	delete _colormap;
	delete _splatmap;
	delete _framebuffer;
	delete _colorbuffer;
	delete _depth;
	delete _renderers;
}


/*float SmoothTerrainSurface::InterpolateHeight(glm::vec2 position) const
{
	return _smoothGroundMap->GetHeightMap()->InterpolateHeight(position);
}*/


/*const float* SmoothTerrainSurface::Intersect(ray r)
{
	return _smoothGroundMap->GetHeightMap()->Intersect(r);
}*/


void SmoothTerrainRenderer::Render(const glm::mat4& transform, const glm::vec3& lightNormal)
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	glm::vec4 map_bounds = glm::vec4(bounds.min, bounds.size());

	glDepthMask(false);

	terrain_uniforms shadow_uniforms;
	shadow_uniforms._transform = transform;
	shadow_uniforms._map_bounds = map_bounds;

	_renderers->render_ground_shadow(_gc, _shadowVertices, shadow_uniforms);

	glDepthMask(true);

	terrain_uniforms uniforms;
	uniforms._transform = transform;
	uniforms._map_bounds = map_bounds;
	uniforms._light_normal = lightNormal;
	uniforms._colormap = _colormap;
	uniforms._splatmap = _splatmap;


	if (_framebuffer != nullptr)
	{
		UpdateDepthTextureSize();

		bind_framebuffer binding(*_framebuffer);

		glClear(GL_DEPTH_BUFFER_BIT);

		terrain_uniforms du;
		du._transform = uniforms._transform;
		du._map_bounds = map_bounds;

		_renderers->render_depth_inside(_gc, _insideVertices, du);
		_renderers->render_depth_border(_gc, _borderVertices, du);

		_renderers->render_depth_skirt(_gc, _skirtVertices, uniforms._transform);
	}

	_renderers->render_terrain_inside(_gc, _insideVertices, uniforms);
	_renderers->render_terrain_border(_gc, _borderVertices, uniforms);

	if (_showLines)
	{
		glDisable(GL_DEPTH_TEST);
		RenderCall<PlainShader_3f>(_gc)
			.SetVertices(&_lineVertices, "position")
			.SetUniform("transform", uniforms._transform)
			.SetUniform("point_size", 1)
			.SetUniform("color", glm::vec4(0, 0, 0, 0.06f))
			.Render();
		glEnable(GL_DEPTH_TEST);
	}

	_renderers->render_terrain_skirt(_gc, _skirtVertices, uniforms._transform, _colormap);

	if (_depth != nullptr)
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);

		VertexShape_2f_2f vertices;
		vertices._mode = GL_TRIANGLE_STRIP;
		vertices.AddVertex(Vertex_2f_2f(glm::vec2(-1, 1), glm::vec2(0, 1)));
		vertices.AddVertex(Vertex_2f_2f(glm::vec2(-1, -1), glm::vec2(0, 0)));
		vertices.AddVertex(Vertex_2f_2f(glm::vec2(1, 1), glm::vec2(1, 1)));
		vertices.AddVertex(Vertex_2f_2f(glm::vec2(1, -1), glm::vec2(1, 0)));

		sobel_uniforms su;
		su._transform = glm::mat4();
		su._depth = _depth;
		_renderers->render_sobel_filter(_gc, vertices, su);

		glDepthMask(true);
		glEnable(GL_DEPTH_TEST);
	}
}



#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES // detect objective-c
static NSString* FramebufferStatusString(GLenum status)
{
	switch (status)
	{
		case GL_FRAMEBUFFER_COMPLETE:
			return @"GL_FRAMEBUFFER_COMPLETE";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			return @"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			return @"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: return @"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
#endif
		case GL_FRAMEBUFFER_UNSUPPORTED:
			return @"GL_FRAMEBUFFER_UNSUPPORTED";
		default:
			return [NSString stringWithFormat:@"0x%04x", (unsigned int)status];
	}
}
#endif



void SmoothTerrainRenderer::EnableRenderEdges()
{
	_depth = new TextureAtlas(_gc);
	glBindTexture(GL_TEXTURE_2D, _depth->_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	UpdateDepthTextureSize();

	_framebuffer = new FrameBuffer();

#if !TARGET_OS_IPHONE
	_colorbuffer = new RenderBuffer(GL_RGBA, _framebuffer_width, _framebuffer_height);
	_framebuffer->AttachColor(_colorbuffer);
#endif

	_framebuffer->AttachDepth(_depth);
	{
		bind_framebuffer binding(*_framebuffer);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
			NSLog(@"CheckGLFramebuffer %@", FramebufferStatusString(status));
#endif
			delete _depth;
			_depth = nullptr;
		}
	}

	if (_depth == nullptr)
	{
		delete _framebuffer;
		_framebuffer = nullptr;

		delete _colorbuffer;
		_colorbuffer = nullptr;
	}
}



void SmoothTerrainRenderer::UpdateDepthTextureSize()
{
	if (_depth != nullptr)
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		if (viewport[2] != _framebuffer_width || viewport[3] != _framebuffer_height)
		{
			_framebuffer_width = viewport[2];
			_framebuffer_height = viewport[3];

			glBindTexture(GL_TEXTURE_2D, _depth->_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _framebuffer_width, _framebuffer_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);

			if (_colorbuffer != nullptr)
				_colorbuffer->Resize(GL_RGBA, _framebuffer_width, _framebuffer_height);
		}
	}
}


void SmoothTerrainRenderer::InitializeShadow()
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	glm::vec2 center = bounds.mid();
	float radius1 = bounds.x().size() / 2;
	float radius2 = radius1 * 1.075f;

	_shadowVertices._mode = GL_TRIANGLES;
	_shadowVertices.Clear();

	int n = 16;
	for (int i = 0; i < n; ++i)
	{
		float angle1 = i * 2 * (float)M_PI / n;
		float angle2 = (i + 1) * 2 * (float)M_PI / n;

		glm::vec2 p1 = center + radius1 * vector2_from_angle(angle1);
		glm::vec2 p2 = center + radius2 * vector2_from_angle(angle1);
		glm::vec2 p3 = center + radius2 * vector2_from_angle(angle2);
		glm::vec2 p4 = center + radius1 * vector2_from_angle(angle2);

		_shadowVertices.AddVertex(Vertex_2f(p1));
		_shadowVertices.AddVertex(Vertex_2f(p2));
		_shadowVertices.AddVertex(Vertex_2f(p3));
		_shadowVertices.AddVertex(Vertex_2f(p3));
		_shadowVertices.AddVertex(Vertex_2f(p4));
		_shadowVertices.AddVertex(Vertex_2f(p1));
	}
}


void SmoothTerrainRenderer::InitializeSkirt()
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	glm::vec2 center = bounds.mid();
	float radius = bounds.x().size() / 2;

	_skirtVertices._mode = GL_TRIANGLE_STRIP;
	_skirtVertices.Clear();

	int n = 1024;
	float d = 2 * (float)M_PI / n;
	for (int i = 0; i < n; ++i)
	{
		float a = d * i;
		glm::vec2 p = center + radius * vector2_from_angle(a);
		float h = fmaxf(0, _smoothGroundMap->GetHeightMap()->InterpolateHeight(p));

		_skirtVertices.AddVertex(Vertex_3f_1f(glm::vec3(p, h + 0.5), h));
		_skirtVertices.AddVertex(Vertex_3f_1f(glm::vec3(p, -2.5), h));
	}

	_skirtVertices.AddVertex(_skirtVertices.GetVertices()[0]);
	_skirtVertices.AddVertex(_skirtVertices.GetVertices()[1]);
}


void SmoothTerrainRenderer::UpdateSplatmap()
{
	int width = _smoothGroundMap->GetImage()->GetWidth();
	int height = _smoothGroundMap->GetImage()->GetHeight();

	GLubyte* data = new GLubyte[4 * width * height];
	if (data != nullptr)
	{
		GLubyte* p = data;
		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
			{
				float forest = _smoothGroundMap->GetForestValue(x, y);
				float block = _smoothGroundMap->GetImpassableValue(x, y);
				*p++ = (GLubyte)(255.0f * block);
				*p++ = (GLubyte)(255.0f * forest);
				*p++ = (GLubyte)(255.0f * forest);
				*p++ = (GLubyte)(255.0f * forest);
			}

		glBindTexture(GL_TEXTURE_2D, _splatmap->_id);
		CHECK_ERROR_GL();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		CHECK_ERROR_GL();
		glGenerateMipmap(GL_TEXTURE_2D);
		CHECK_ERROR_GL();

		delete[] data;
	}
}


void SmoothTerrainRenderer::UpdateChanges(bounds2f bounds)
{
	InitializeSkirt();
	UpdateSplatmap();

	if (_insideVertices.GetVertices().empty())
	{
		_editMode = true;
		BuildTriangles();
	}
	else
	{
		// inside
		for (Vertex_3f_3f& vertex : _insideVertices.GetMutableVertices())
		{
			glm::vec2 p = GetVertexAttribute<0>(vertex).xy();
			if (bounds.contains(p))
			{
				glm::ivec2 i = _smoothGroundMap->ToGroundmapCoordinate(p);
				GetVertexAttribute<0>(vertex).z = _smoothGroundMap->GetHeightMap()->GetHeight(i.x, i.y);
				GetVertexAttribute<1>(vertex) = _smoothGroundMap->GetHeightMap()->GetNormal(i.x, i.y);
			}
		}

		// border
		for (Vertex_3f_3f& vertex : _borderVertices.GetMutableVertices())
		{
			glm::vec2 p = GetVertexAttribute<0>(vertex).xy();
			if (bounds.contains(p))
			{
				glm::ivec2 i = _smoothGroundMap->ToGroundmapCoordinate(p);
				GetVertexAttribute<0>(vertex).z = _smoothGroundMap->GetHeightMap()->GetHeight(i.x, i.y);
				GetVertexAttribute<1>(vertex) = _smoothGroundMap->GetHeightMap()->GetNormal(i.x, i.y);
			}
		}
	}

	// lines
	if (_showLines)
	{
		for (Vertex_3f& vertex : _lineVertices.GetMutableVertices())
		{
			glm::vec2 p = vertex._v.xy();
			if (bounds.contains(p))
			{
				vertex._v.z = _smoothGroundMap->GetHeightMap()->InterpolateHeight(p);
			}
		}
	}

	// skirt
	for (size_t i = 0; i < _skirtVertices.GetMutableVertices().size(); i += 2)
	{
		glm::vec2 p = GetVertexAttribute<0>(_skirtVertices.GetMutableVertices()[i]).xy();
		if (bounds.contains(p))
		{
			float h = fmaxf(0, _smoothGroundMap->GetHeightMap()->InterpolateHeight(p));
			GetVertexAttribute<1>(_skirtVertices.GetMutableVertices()[i]) = h;
			GetVertexAttribute<0>(_skirtVertices.GetMutableVertices()[i]).z = h;
		}
	}
}


void SmoothTerrainRenderer::InitializeLines()
{
	if (_showLines)
	{
		bounds2f bounds = _smoothGroundMap->GetBounds();
		glm::vec2 corner = bounds.min;
		glm::vec2 size = bounds.size();

		_lineVertices._mode = GL_LINES;
		_lineVertices.Clear();
		int n = _smoothGroundMap->GetHeightMap()->GetMaxIndex();
		float k = _smoothGroundMap->GetImage()->GetWidth();
		for (int x = 0; x <= n; x += 2)
		{
			for (int y = 0; y <= n; y += 2)
			{
				float x0 = corner.x + size.x * (x / k);
				float y0 = corner.y + size.y * (y / k);
				float h00 = _smoothGroundMap->GetHeightMap()->GetHeight(x, y);

				float x2, h20;
				if (x != n)
				{
					x2 = corner.x + size.x * ((x + 2) / k);
					h20 = _smoothGroundMap->GetHeightMap()->GetHeight(x + 2, y);
					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x0, y0, h00)));
					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x2, y0, h20)));
				}
				float y2, h02;
				if (y != n)
				{
					y2 = corner.y + size.y * ((y + 2) / k);
					h02 = _smoothGroundMap->GetHeightMap()->GetHeight(x, y + 2);
					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x0, y0, h00)));
					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x0, y2, h02)));
				}

				if (x != n && y != n)
				{
					float x1 = corner.x + size.x * ((x + 1) / k);
					float y1 = corner.y + size.y * ((y + 1) / k);
					float h11 = _smoothGroundMap->GetHeightMap()->GetHeight(x + 1, y + 1);
					float h22 = _smoothGroundMap->GetHeightMap()->GetHeight(x + 2, y + 2);

					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x0, y0, h00)));
					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x1, y1, h11)));

					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x2, y0, h20)));
					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x1, y1, h11)));

					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x0, y2, h02)));
					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x1, y1, h11)));

					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x2, y2, h22)));
					_lineVertices.AddVertex(Vertex_3f(glm::vec3(x1, y1, h11)));
				}
			}
		}
	}
}


static int inside_circle(bounds2f bounds, glm::vec2 p)
{
	return glm::length(p - bounds.mid()) <= bounds.x().size() / 2 ? 1 : 0;
}


static int inside_circle(bounds2f bounds, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3)
{
	return inside_circle(bounds, v1)
		+ inside_circle(bounds, v2)
		+ inside_circle(bounds, v3);

}


void SmoothTerrainRenderer::BuildTriangles()
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	glm::vec2 corner = bounds.mix_00();
	glm::vec2 size = bounds.size();

	_insideVertices._mode = GL_TRIANGLES;
	_insideVertices.Clear();

	_borderVertices._mode = GL_TRIANGLES;
	_borderVertices.Clear();

	int n = _smoothGroundMap->GetHeightMap()->GetMaxIndex();
	float k = _smoothGroundMap->GetImage()->GetWidth();

	for (int x = 0; x < n; x += 2)
		for (int y = 0; y < n; y += 2)
		{
			float x0 = corner.x + size.x * (x / k);
			float x1 = corner.x + size.x * ((x + 1) / k);
			float x2 = corner.x + size.x * ((x + 2) / k);
			float y0 = corner.y + size.y * (y / k);
			float y1 = corner.y + size.y * ((y + 1) / k);
			float y2 = corner.y + size.y * ((y + 2) / k);

			float h00 = _smoothGroundMap->GetHeightMap()->GetHeight(x, y);
			float h02 = _smoothGroundMap->GetHeightMap()->GetHeight(x, y + 2);
			float h20 = _smoothGroundMap->GetHeightMap()->GetHeight(x + 2, y);
			float h11 = _smoothGroundMap->GetHeightMap()->GetHeight(x + 1, y + 1);
			float h22 = _smoothGroundMap->GetHeightMap()->GetHeight(x + 2, y + 2);

			glm::vec3 n00 = _smoothGroundMap->GetHeightMap()->GetNormal(x, y);
			glm::vec3 n02 = _smoothGroundMap->GetHeightMap()->GetNormal(x, y + 2);
			glm::vec3 n20 = _smoothGroundMap->GetHeightMap()->GetNormal(x + 2, y);
			glm::vec3 n11 = _smoothGroundMap->GetHeightMap()->GetNormal(x + 1, y + 1);
			glm::vec3 n22 = _smoothGroundMap->GetHeightMap()->GetNormal(x + 2, y + 2);

			Vertex_3f_3f v00 = Vertex_3f_3f(glm::vec3(x0, y0, h00), n00);
			Vertex_3f_3f v02 = Vertex_3f_3f(glm::vec3(x0, y2, h02), n02);
			Vertex_3f_3f v20 = Vertex_3f_3f(glm::vec3(x2, y0, h20), n20);
			Vertex_3f_3f v11 = Vertex_3f_3f(glm::vec3(x1, y1, h11), n11);
			Vertex_3f_3f v22 = Vertex_3f_3f(glm::vec3(x2, y2, h22), n22);

			PushTriangle(v00, v20, v11);
			PushTriangle(v20, v22, v11);
			PushTriangle(v22, v02, v11);
			PushTriangle(v02, v00, v11);
		}

	/*if (!_editMode)
	{
		// be nice and free allocated memory

		_insideVertices.Clear();
		_insideVertices._vertices.shrink_to_fit();

		_borderVertices.Clear();
		_borderVertices._vertices.shrink_to_fit();
	}*/
}


void SmoothTerrainRenderer::PushTriangle(const Vertex_3f_3f& v0, const Vertex_3f_3f& v1, const Vertex_3f_3f& v2)
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	bool inside = inside_circle(bounds,
		GetVertexAttribute<0>(v0).xy(),
		GetVertexAttribute<0>(v1).xy(),
		GetVertexAttribute<0>(v2).xy());
	VertexShape_3f_3f* vertices = SelectTerrainVertexBuffer(inside);
	if (vertices != nullptr)
	{
		vertices->AddVertex(v0);
		vertices->AddVertex(v1);
		vertices->AddVertex(v2);
	}
}


VertexShape_3f_3f* SmoothTerrainRenderer::SelectTerrainVertexBuffer(int inside)
{
	switch (inside)
	{
		case 1:
		case 2:
			return &_borderVertices;
		case 3:
			return &_insideVertices;
		default:
			return nullptr;
	}
}
