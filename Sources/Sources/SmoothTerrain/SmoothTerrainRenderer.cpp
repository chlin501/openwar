// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "../../Library/Algebra/image.h"
#include "SmoothTerrainRenderer.h"



SmoothTerrainRenderer::SmoothTerrainRenderer(SmoothGroundMap* smoothGroundMap) :
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
	_colormap = SmoothTerrainShaders::create_colormap();

	_splatmap = new texture();
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


void SmoothTerrainRenderer::Render(const glm::mat4x4& transform, const glm::vec3& lightNormal)
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	glm::vec4 map_bounds = glm::vec4(bounds.min, bounds.size());

	glDepthMask(false);

	terrain_uniforms shadow_uniforms;
	shadow_uniforms._transform = transform;
	shadow_uniforms._map_bounds = map_bounds;

	_renderers->render_ground_shadow(_vboShadow, shadow_uniforms);

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

		_renderers->render_depth_inside(_vboInside, du);
		_renderers->render_depth_border(_vboBorder, du);

		_renderers->render_depth_skirt(_vboSkirt, uniforms._transform);
	}

	_renderers->render_terrain_inside(_vboInside, uniforms);
	_renderers->render_terrain_border(_vboBorder, uniforms);

	if (_showLines)
	{
		glDisable(GL_DEPTH_TEST);
		renderers::singleton->_plain_renderer3->get_uniform<glm::mat4>("transform").set_value(uniforms._transform);
		renderers::singleton->_plain_renderer3->get_uniform<float>("point_size").set_value(1);
		renderers::singleton->_plain_renderer3->get_uniform<glm::vec4>("color").set_value(glm::vec4(0, 0, 0, 0.06f));
		renderers::singleton->_plain_renderer3->render(_vboLines);
		glEnable(GL_DEPTH_TEST);
	}

	_renderers->render_terrain_skirt(_vboSkirt, uniforms._transform, _colormap);

	if (_depth != nullptr)
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);

		vertexbuffer<texture_vertex> shape;
		shape._mode = GL_TRIANGLE_STRIP;
		shape._vertices.push_back(texture_vertex(glm::vec2(-1, 1), glm::vec2(0, 1)));
		shape._vertices.push_back(texture_vertex(glm::vec2(-1, -1), glm::vec2(0, 0)));
		shape._vertices.push_back(texture_vertex(glm::vec2(1, 1), glm::vec2(1, 1)));
		shape._vertices.push_back(texture_vertex(glm::vec2(1, -1), glm::vec2(1, 0)));

		sobel_uniforms su;
		su._transform = glm::mat4x4();
		su._depth = _depth;
		_renderers->render_sobel_filter(shape, su);

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
	_depth = new texture();
	glBindTexture(GL_TEXTURE_2D, _depth->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	UpdateDepthTextureSize();

	_framebuffer = new framebuffer();

#if !TARGET_OS_IPHONE
	_colorbuffer = new renderbuffer(GL_RGBA, _framebuffer_width, _framebuffer_height);
	_framebuffer->attach_color(_colorbuffer);
#endif

	_framebuffer->attach_depth(_depth);
	{
		bind_framebuffer binding(*_framebuffer);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
#ifdef OPENWAR_USE_NSBUNDLE_RESOURCES
			NSLog(@"CheckGLFramebuffer %@", FramebufferStatusString(status));
#endif
		}
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

			glBindTexture(GL_TEXTURE_2D, _depth->id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _framebuffer_width, _framebuffer_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);

			if (_colorbuffer != nullptr)
				_colorbuffer->resize(GL_RGBA, _framebuffer_width, _framebuffer_height);
		}
	}
}


void SmoothTerrainRenderer::InitializeShadow()
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	glm::vec2 center = bounds.center();
	float radius1 = bounds.width() / 2;
	float radius2 = radius1 * 1.075f;

	_vboShadow._mode = GL_TRIANGLES;
	_vboShadow._vertices.clear();

	int n = 16;
	for (int i = 0; i < n; ++i)
	{
		float angle1 = i * 2 * (float)M_PI / n;
		float angle2 = (i + 1) * 2 * (float)M_PI / n;

		glm::vec2 p1 = center + radius1 * vector2_from_angle(angle1);
		glm::vec2 p2 = center + radius2 * vector2_from_angle(angle1);
		glm::vec2 p3 = center + radius2 * vector2_from_angle(angle2);
		glm::vec2 p4 = center + radius1 * vector2_from_angle(angle2);

		_vboShadow._vertices.push_back(plain_vertex(p1));
		_vboShadow._vertices.push_back(plain_vertex(p2));
		_vboShadow._vertices.push_back(plain_vertex(p3));
		_vboShadow._vertices.push_back(plain_vertex(p3));
		_vboShadow._vertices.push_back(plain_vertex(p4));
		_vboShadow._vertices.push_back(plain_vertex(p1));
	}

	_vboShadow.update(GL_STATIC_DRAW);
}


void SmoothTerrainRenderer::InitializeSkirt()
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	glm::vec2 center = bounds.center();
	float radius = bounds.width() / 2;

	_vboSkirt._mode = GL_TRIANGLE_STRIP;
	_vboSkirt._vertices.clear();

	int n = 1024;
	float d = 2 * (float)M_PI / n;
	for (int i = 0; i < n; ++i)
	{
		float a = d * i;
		glm::vec2 p = center + radius * vector2_from_angle(a);
		float h = fmaxf(0, _smoothGroundMap->GetHeightMap()->InterpolateHeight(p));

		_vboSkirt._vertices.push_back(skirt_vertex(glm::vec3(p, h + 0.5), h));
		_vboSkirt._vertices.push_back(skirt_vertex(glm::vec3(p, -2.5), h));
	}

	_vboSkirt._vertices.push_back(_vboSkirt._vertices[0]);
	_vboSkirt._vertices.push_back(_vboSkirt._vertices[1]);

	_vboSkirt.update(GL_STATIC_DRAW);
}


void SmoothTerrainRenderer::UpdateSplatmap()
{
	glm::ivec2 size = _smoothGroundMap->GetImage()->size();

	GLubyte* data = new GLubyte[4 * size.x * size.y];
	if (data != nullptr)
	{
		GLubyte* p = data;
		for (int y = 0; y < size.y; ++y)
			for (int x = 0; x < size.x; ++x)
			{
				float forest = _smoothGroundMap->GetForestValue(x, y);
				float block = _smoothGroundMap->GetImpassableValue(x, y);
				*p++ = (GLubyte)(255.0f * block);
				*p++ = (GLubyte)(255.0f * forest);
				*p++ = (GLubyte)(255.0f * forest);
				*p++ = (GLubyte)(255.0f * forest);
			}

		glBindTexture(GL_TEXTURE_2D, _splatmap->id);
		CHECK_ERROR_GL();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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

	if (_vboInside._vertices.empty())
	{
		_editMode = true;
		BuildTriangles();
	}
	else
	{
		// inside
		for (terrain_vertex& vertex : _vboInside._vertices)
		{
			glm::vec2 p = vertex._position.xy();
			if (bounds.contains(p))
			{
				glm::ivec2 i = _smoothGroundMap->ToGroundmapCoordinate(p);
				vertex._position.z = _smoothGroundMap->GetHeightMap()->GetHeight(i.x, i.y);
				vertex._normal = _smoothGroundMap->GetHeightMap()->GetNormal(i.x, i.y);
			}
		}
		_vboInside.update(GL_STATIC_DRAW);

		// border
		for (terrain_vertex& vertex : _vboBorder._vertices)
		{
			glm::vec2 p = vertex._position.xy();
			if (bounds.contains(p))
			{
				glm::ivec2 i = _smoothGroundMap->ToGroundmapCoordinate(p);
				vertex._position.z = _smoothGroundMap->GetHeightMap()->GetHeight(i.x, i.y);
				vertex._normal = _smoothGroundMap->GetHeightMap()->GetNormal(i.x, i.y);
			}
		}
		_vboBorder.update(GL_STATIC_DRAW);
	}

	// lines
	if (_showLines)
	{
		for (plain_vertex3& vertex : _vboLines._vertices)
		{
			glm::vec2 p = vertex._position.xy();
			if (bounds.contains(p))
			{
				vertex._position.z = _smoothGroundMap->GetHeightMap()->InterpolateHeight(p);
			}
		}
		_vboLines.update(GL_STATIC_DRAW);
	}

	// skirt
	for (size_t i = 0; i < _vboSkirt._vertices.size(); i += 2)
	{
		glm::vec2 p = _vboSkirt._vertices[i]._position.xy();
		if (bounds.contains(p))
		{
			float h = fmaxf(0, _smoothGroundMap->GetHeightMap()->InterpolateHeight(p));
			_vboSkirt._vertices[i]._height = h;
			_vboSkirt._vertices[i]._position.z = h;
		}
	}
	_vboSkirt.update(GL_STATIC_DRAW);
}


void SmoothTerrainRenderer::InitializeLines()
{
	if (_showLines)
	{
		bounds2f bounds = _smoothGroundMap->GetBounds();
		glm::vec2 corner = bounds.min;
		glm::vec2 size = bounds.size();

		_vboLines._mode = GL_LINES;
		_vboLines._vertices.clear();
		int n = _smoothGroundMap->GetHeightMap()->GetMaxIndex();
		float k = _smoothGroundMap->GetImage()->size().x;
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
					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x0, y0, h00)));
					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x2, y0, h20)));
				}
				float y2, h02;
				if (y != n)
				{
					y2 = corner.y + size.y * ((y + 2) / k);
					h02 = _smoothGroundMap->GetHeightMap()->GetHeight(x, y + 2);
					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x0, y0, h00)));
					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x0, y2, h02)));
				}

				if (x != n && y != n)
				{
					float x1 = corner.x + size.x * ((x + 1) / k);
					float y1 = corner.y + size.y * ((y + 1) / k);
					float h11 = _smoothGroundMap->GetHeightMap()->GetHeight(x + 1, y + 1);
					float h22 = _smoothGroundMap->GetHeightMap()->GetHeight(x + 2, y + 2);

					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x0, y0, h00)));
					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x1, y1, h11)));

					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x2, y0, h20)));
					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x1, y1, h11)));

					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x0, y2, h02)));
					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x1, y1, h11)));

					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x2, y2, h22)));
					_vboLines._vertices.push_back(plain_vertex3(glm::vec3(x1, y1, h11)));
				}
			}
		}
		_vboLines.update(GL_STATIC_DRAW);
	}
}


static int inside_circle(bounds2f bounds, glm::vec2 p)
{
	return glm::length(p - bounds.center()) <= bounds.width() / 2 ? 1 : 0;
}


static int inside_circle(bounds2f bounds, terrain_vertex v1, terrain_vertex v2, terrain_vertex v3)
{
	return inside_circle(bounds, v1._position.xy())
		+ inside_circle(bounds, v2._position.xy())
		+ inside_circle(bounds, v3._position.xy());

}


void SmoothTerrainRenderer::BuildTriangles()
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	glm::vec2 corner = bounds.p11();
	glm::vec2 size = bounds.size();

	_vboInside._mode = GL_TRIANGLES;
	_vboInside._vertices.clear();

	_vboBorder._mode = GL_TRIANGLES;
	_vboBorder._vertices.clear();

	int n = _smoothGroundMap->GetHeightMap()->GetMaxIndex();
	float k = _smoothGroundMap->GetImage()->size().x;

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

			terrain_vertex v00 = terrain_vertex(glm::vec3(x0, y0, h00), n00);
			terrain_vertex v02 = terrain_vertex(glm::vec3(x0, y2, h02), n02);
			terrain_vertex v20 = terrain_vertex(glm::vec3(x2, y0, h20), n20);
			terrain_vertex v11 = terrain_vertex(glm::vec3(x1, y1, h11), n11);
			terrain_vertex v22 = terrain_vertex(glm::vec3(x2, y2, h22), n22);

			PushTriangle(v00, v20, v11);
			PushTriangle(v20, v22, v11);
			PushTriangle(v22, v02, v11);
			PushTriangle(v02, v00, v11);
		}

	_vboInside.update(GL_STATIC_DRAW);
	_vboBorder.update(GL_STATIC_DRAW);

	if (!_editMode)
	{
		// be nice and free allocated memory

		_vboInside._vertices.clear();
		_vboInside._vertices.shrink_to_fit();

		_vboBorder._vertices.clear();
		_vboBorder._vertices.shrink_to_fit();
	}
}


void SmoothTerrainRenderer::PushTriangle(const terrain_vertex& v0, const terrain_vertex& v1, const terrain_vertex& v2)
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	vertexbuffer<terrain_vertex>* s = SelectTerrainVbo(inside_circle(bounds, v0, v1, v2));
	if (s != nullptr)
	{
		s->_vertices.push_back(v0);
		s->_vertices.push_back(v1);
		s->_vertices.push_back(v2);
	}
}


vertexbuffer<terrain_vertex>* SmoothTerrainRenderer::SelectTerrainVbo(int inside)
{
	switch (inside)
	{
		case 1:
		case 2:
			return &_vboBorder;
		case 3:
			return &_vboInside;
		default:
			return nullptr;
	}
}
