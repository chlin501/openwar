// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SmoothTerrainRenderer.h"
#include "Graphics/Image.h"
#include "Graphics/CommonShaders.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderBuffer.h"
#include "Graphics/TextureResource.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>

#ifdef __ANDROID__
#include <android/log.h>
#endif


SmoothTerrainRenderer::SmoothTerrainRenderer(GraphicsContext* gc, SmoothGroundMap* smoothGroundMap) :
_gc(gc),
_smoothGroundMap(smoothGroundMap)
{
	_colormap = CreateColorMap(gc);

	_splatmap = new TextureAtlas(gc);
	UpdateSplatmap();

	InitializeSkirt();
	InitializeShadow();
	InitializeLines();

	BuildTriangles();
};


SmoothTerrainRenderer::~SmoothTerrainRenderer()
{
	delete _colormap;
	delete _splatmap;

	delete _sobelFrameBuffer;
	delete _sobelColorBuffer;
	delete _sobelDepthBuffer;

	delete _hatchingsMasterColorBuffer;
	delete _hatchingsMasterFrameBuffer;

	delete _hatchingsIntermediateFrameBuffer;
	delete _hatchingsIntermediateColorBuffer;
	delete _hatchingsIntermediateDepthBuffer;

	delete _hatchingsDeployment;
	delete _hatchingsPatternR;
	delete _hatchingsPatternG;
	delete _hatchingsPatternB;
}


void SmoothTerrainRenderer::SetDeploymentZoneBlue(glm::vec2 position, float radius)
{
	_deploymentPositionBlue = position;
	_deploymentRadiusBlue = radius;

	TryEnableHatchingsBuffers();
}


void SmoothTerrainRenderer::SetDeploymentZoneRed(glm::vec2 position, float radius)
{
	_deploymentPositionRed = position;
	_deploymentRadiusRed = radius;

	TryEnableHatchingsBuffers();
}


void SmoothTerrainRenderer::Render(const glm::mat4& transform, const glm::vec3& lightNormal)
{
	RenderGroundShadow(transform);
	UpdateSobelTexture(transform);
	RenderTerrain(transform, lightNormal);
	RenderSobelTexture();
	RenderLines(transform);
};


void SmoothTerrainRenderer::RenderGroundShadow(const glm::mat4& transform)
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	glm::vec4 map_bounds = glm::vec4(bounds.min, bounds.size());

	RenderCall<GroundShadowShader>(_gc)
		.SetVertices(&_shadowVertices, "position")
		.SetUniform("transform", transform)
		.SetUniform("map_bounds", map_bounds)
		.SetCullBack(true)
		.ClearDepth()
		.Render();
}


void SmoothTerrainRenderer::RenderTerrain(const glm::mat4& transform, const glm::vec3& lightNormal)
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	glm::vec4 map_bounds = glm::vec4(bounds.min, bounds.size());

	RenderCall<TerrainInsideShader>(_gc)
		.SetVertices(&_insideVertices, "position", "normal")
		.SetUniform("transform", transform)
		.SetUniform("light_normal", lightNormal)
		.SetUniform("map_bounds", map_bounds)
		.SetTexture("colormap", _colormap, Sampler(SamplerMinMagFilter::Linear, SamplerAddressMode::Clamp))
		.SetTexture("splatmap", _splatmap)
		.SetDepthTest(true)
		.SetDepthMask(true)
		.SetCullBack(true)
		.Render();

	RenderCall<TerrainBorderShader>(_gc)
		.SetVertices(&_borderVertices, "position", "normal")
		.SetUniform("transform", transform)
		.SetUniform("light_normal", lightNormal)
		.SetUniform("map_bounds", map_bounds)
		.SetTexture("colormap", _colormap, Sampler(SamplerMinMagFilter::Linear, SamplerAddressMode::Clamp))
		.SetTexture("splatmap", _splatmap)
		.SetDepthTest(true)
		.SetDepthMask(true)
		.SetCullBack(true)
		.Render();

	RenderCall<TerrainSkirtShader>(_gc)
		.SetVertices(&_skirtVertices, "position", "height")
		.SetUniform("transform", transform)
		.SetTexture("texture", _colormap, Sampler(SamplerMinMagFilter::Linear, SamplerAddressMode::Clamp))
		.SetDepthTest(true)
		.SetDepthMask(true)
		.SetCullBack(true)
		.Render();
}


void SmoothTerrainRenderer::RenderLines(const glm::mat4& transform)
{
	if (_showLines)
	{
		RenderCall<PlainShader_3f>(_gc)
			.SetVertices(&_lineVertices, "position")
			.SetUniform("transform", transform)
			.SetUniform("point_size", 1.0f)
			.SetUniform("color", glm::vec4(0, 0, 0, 0.06f))
			.Render();
	}
}


void SmoothTerrainRenderer::EnableSobelBuffers()
{
	_sobelFrameBuffer = new FrameBuffer();

#if TARGET_OS_MAC
	_sobelColorBuffer = new RenderBuffer();
#endif
	_sobelDepthBuffer = new Texture(_gc);

	UpdateSobelBufferSize();

#if TARGET_OS_MAC
	_sobelFrameBuffer->AttachColor(_sobelColorBuffer);
#endif
	_sobelFrameBuffer->AttachDepth(_sobelDepthBuffer);


	if (!_sobelFrameBuffer->IsComplete())
	{
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
		NSLog(@"EnableSobelBuffers: _sobelFrameBuffer=%s", _sobelFrameBuffer->GetStatus());
#endif
#ifdef __ANDROID__
		__android_log_print(ANDROID_LOG_INFO, "openwar", "EnableSobelBuffers: _sobelFrameBuffer=%s", _sobelFrameBuffer->GetStatus());
#endif

		delete _sobelFrameBuffer;
		_sobelFrameBuffer = nullptr;

		delete _sobelDepthBuffer;
		_sobelDepthBuffer = nullptr;

		delete _sobelColorBuffer;
		_sobelColorBuffer = nullptr;
	}
}


void SmoothTerrainRenderer::UpdateSobelBufferSize()
{
	bounds2i viewport = _gc->GetViewport();
	glm::ivec2 size = viewport.size();

	if (size.x != _framebuffer_width || size.y != _framebuffer_height)
	{
		_framebuffer_width = size.x;
		_framebuffer_height = size.y;

		if (_sobelColorBuffer != nullptr)
			_sobelColorBuffer->PrepareColorBuffer(_framebuffer_width, _framebuffer_height);

		if (_sobelDepthBuffer != nullptr)
			_sobelDepthBuffer->PrepareDepthBuffer(_framebuffer_width, _framebuffer_height);
	}
}


void SmoothTerrainRenderer::UpdateSobelTexture(const glm::mat4& transform)
{
	bounds2f bounds = _smoothGroundMap->GetBounds();
	glm::vec4 map_bounds = glm::vec4(bounds.min, bounds.size());

	if (_sobelFrameBuffer != nullptr)
	{
		UpdateSobelBufferSize();

		RenderCall<DepthInsideShader>(_gc)
			.SetFrameBuffer(_sobelFrameBuffer)
			.SetVertices(&_insideVertices, "position", "normal")
			.SetUniform("transform", transform)
			.SetDepthTest(true)
			.SetDepthMask(true)
			.SetCullBack(true)
			.ClearDepth()
			.Render();

		RenderCall<DepthBorderShader>(_gc)
			.SetFrameBuffer(_sobelFrameBuffer)
			.SetVertices(&_borderVertices, "position", "normal")
			.SetUniform("transform", transform)
			.SetUniform("map_bounds", map_bounds)
			.SetDepthTest(true)
			.SetDepthMask(true)
			.SetCullBack(true)
			.Render();

		RenderCall<DepthSkirtShader>(_gc)
			.SetFrameBuffer(_sobelFrameBuffer)
			.SetVertices(&_skirtVertices, "position", "height")
			.SetUniform("transform", transform)
			.SetDepthTest(true)
			.SetDepthMask(true)
			.SetCullBack(true)
			.Render();
	}
}


void SmoothTerrainRenderer::RenderSobelTexture()
{
	if (_sobelDepthBuffer != nullptr)
	{
		VertexShape_2f_2f vertices;
		vertices._mode = GL_TRIANGLE_STRIP;
		vertices.AddVertex({{-1, 1}, {0, 1}});
		vertices.AddVertex({{-1, -1}, {0, 0}});
		vertices.AddVertex({{1, 1}, {1, 1}});
		vertices.AddVertex({{1, -1}, {1, 0}});

		RenderCall<SobelFilterShader>(_gc)
			.SetVertices(&vertices, "position", "texcoord")
			.SetUniform("transform", glm::mat4())
			.SetTexture("depth", _sobelDepthBuffer, Sampler(SamplerMinMagFilter::Nearest, SamplerAddressMode::Clamp))
			.Render();
	}
}


void SmoothTerrainRenderer::TryEnableHatchingsBuffers()
{
	if (_hatchingsMasterFrameBuffer == nullptr && (_deploymentRadiusBlue > 0 || _deploymentRadiusRed > 0))
	{
		// Hatchings Master Buffer

		_hatchingsMasterBufferSize = glm::ivec2{128, 128};

		_hatchingsMasterColorBuffer = new Texture(_gc);
		_hatchingsMasterColorBuffer->PrepareColorBuffer(_hatchingsMasterBufferSize.x, _hatchingsMasterBufferSize.y);

		_hatchingsMasterFrameBuffer = new FrameBuffer();
		_hatchingsMasterFrameBuffer->AttachColor(_hatchingsMasterColorBuffer);
		if (!_hatchingsMasterFrameBuffer->IsComplete())
		{
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
			NSLog(@"TryEnableHatchingsBuffers: _hatchingsMasterFrameBuffer %s", _hatchingsMasterFrameBuffer->GetStatus());
#endif
#ifdef __ANDROID__
			__android_log_print(ANDROID_LOG_INFO, "openwar", "TryEnableHatchingsBuffers: _hatchingsMasterFrameBuffer=%s", _hatchingsMasterFrameBuffer->GetStatus());
#endif
		}

		// Hatchings Intermediate Buffer

		_hatchingsIntermediateBufferSize = glm::ivec2{128, 128};

		_hatchingsIntermediateColorBuffer = new Texture(_gc);
		_hatchingsIntermediateDepthBuffer = new RenderBuffer();

		_hatchingsIntermediateColorBuffer->PrepareColorBuffer(_hatchingsIntermediateBufferSize.x, _hatchingsIntermediateBufferSize.y);
		_hatchingsIntermediateDepthBuffer->PrepareDepthBuffer(_hatchingsIntermediateBufferSize.x, _hatchingsIntermediateBufferSize.y);

		_hatchingsIntermediateFrameBuffer = new FrameBuffer();
		_hatchingsIntermediateFrameBuffer->AttachColor(_hatchingsIntermediateColorBuffer);
		_hatchingsIntermediateFrameBuffer->AttachDepth(_hatchingsIntermediateDepthBuffer);
		if (!_hatchingsIntermediateFrameBuffer->IsComplete())
		{
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
			NSLog(@"TryEnableHatchingsBuffers: _hatchingsIntermediateFrameBuffer %s", _hatchingsIntermediateFrameBuffer->GetStatus());
#endif
#ifdef __ANDROID__
			__android_log_print(ANDROID_LOG_INFO, "openwar", "TryEnableHatchingsBuffers: _hatchingsIntermediateFrameBuffer=%s", _hatchingsIntermediateFrameBuffer->GetStatus());
#endif
		}

		_hatchingsResultVertices.Reset(GL_TRIANGLE_STRIP);
		_hatchingsResultVertices.AddVertex({{-1, 1}, {0, 1}});
		_hatchingsResultVertices.AddVertex({{-1, -1}, {0, 0}});
		_hatchingsResultVertices.AddVertex({{1, 1}, {1, 1}});
		_hatchingsResultVertices.AddVertex({{1, -1}, {1, 0}});

		// Hacthing Patterns

		_hatchingsDeployment = new TextureResource(_gc, Resource("Textures/Deployment.png"));
		_hatchingsPatternR = new TextureResource(_gc, Resource("Textures/HatchPatternR.png"));
		_hatchingsPatternG = new TextureResource(_gc, Resource("Textures/HatchPatternG.png"));
		_hatchingsPatternB = new TextureResource(_gc, Resource("Textures/HatchPatternB.png"));
	}
};


void SmoothTerrainRenderer::RenderHatchings(const glm::mat4& transform)
{
	if (_hatchingsMasterFrameBuffer != nullptr && (_deploymentRadiusBlue > 0 || _deploymentRadiusRed > 0))
	{
		GLint oldViewport[4];

		glGetIntegerv(GL_VIEWPORT, oldViewport);

		_hatchingsMasterVertices.Reset(GL_TRIANGLES);

		if (_deploymentRadiusBlue > 0)
		{
			bounds2f b = bounds2f{_deploymentPositionBlue}.grow(_deploymentRadiusBlue * 64.0f / 60.0f);
			_hatchingsMasterVertices.AddVertex({{b.min.x, b.min.y}, {0, 0}});
			_hatchingsMasterVertices.AddVertex({{b.min.x, b.max.y}, {0, 1}});
			_hatchingsMasterVertices.AddVertex({{b.max.x, b.max.y}, {0.5f, 1}});
			_hatchingsMasterVertices.AddVertex({{b.max.x, b.max.y}, {0.5f, 1}});
			_hatchingsMasterVertices.AddVertex({{b.max.x, b.min.y}, {0.5f, 0}});
			_hatchingsMasterVertices.AddVertex({{b.min.x, b.min.y}, {0, 0}});
		}
		if (_deploymentRadiusRed > 0)
		{
			bounds2f b = bounds2f{_deploymentPositionRed}.grow(_deploymentRadiusRed * 64.0f / 60.0f);
			_hatchingsMasterVertices.AddVertex({{b.min.x, b.min.y}, {0.5f, 0}});
			_hatchingsMasterVertices.AddVertex({{b.min.x, b.max.y}, {0.5f, 1}});
			_hatchingsMasterVertices.AddVertex({{b.max.x, b.max.y}, {1, 1}});
			_hatchingsMasterVertices.AddVertex({{b.max.x, b.max.y}, {1, 1}});
			_hatchingsMasterVertices.AddVertex({{b.max.x, b.min.y}, {1, 0}});
			_hatchingsMasterVertices.AddVertex({{b.min.x, b.min.y}, {0.5f, 0}});
		}

		bounds2f bounds = _smoothGroundMap->GetBounds();
		glm::vec3 translate = glm::vec3{-bounds.mid(), 0};
		glm::vec3 scale = glm::vec3{2.0f / bounds.size(), 0};

		glViewport(0, 0, _hatchingsMasterBufferSize.x, _hatchingsMasterBufferSize.y);

		RenderCall<HatchingsMasterShader>(_gc)
			.SetFrameBuffer(_hatchingsMasterFrameBuffer)
			.SetVertices(&_hatchingsMasterVertices, "position", "texcoord")
			.SetUniform("transform", glm::translate(glm::scale(glm::mat4{}, scale), translate))
			.SetTexture("texture", _hatchingsDeployment, Sampler(SamplerMinMagFilter::Linear, SamplerAddressMode::Clamp))
			.ClearColor(glm::vec4{})
			.Render();


		/***/

		glViewport(0, 0, _hatchingsIntermediateBufferSize.x, _hatchingsIntermediateBufferSize.y);
		glm::vec4 map_bounds = glm::vec4{bounds.min, bounds.size()};

		RenderCall<HatchingsInsideShader>(_gc)
			.SetFrameBuffer(_hatchingsIntermediateFrameBuffer)
			.SetVertices(&_insideVertices, "position", "normal")
			.SetUniform("transform", transform)
			.SetUniform("map_bounds", map_bounds)
			.SetTexture("texture", _hatchingsMasterColorBuffer, Sampler(SamplerMinMagFilter::Linear, SamplerAddressMode::Clamp))
			.SetDepthTest(true)
			.SetDepthMask(true)
			.SetCullBack(true)
			.ClearDepth()
			.ClearColor(glm::vec4{0, 0, 0, 1})
			.Render();

		RenderCall<HatchingsBorderShader>(_gc)
			.SetFrameBuffer(_hatchingsIntermediateFrameBuffer)
			.SetVertices(&_borderVertices, "position", "normal")
			.SetUniform("transform", transform)
			.SetUniform("map_bounds", map_bounds)
			.SetTexture("texture", _hatchingsMasterColorBuffer, Sampler(SamplerMinMagFilter::Linear, SamplerAddressMode::Clamp))
			.SetDepthTest(true)
			.SetDepthMask(true)
			.SetCullBack(true)
			.Render();

		/***/

		glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

		RenderCall<HatchingsResultShader>(_gc)
			.SetVertices(&_hatchingsResultVertices, "position", "texcoord")
			.SetUniform("transform", glm::mat4())
			.SetTexture("texture", _hatchingsIntermediateColorBuffer, Sampler(SamplerMinMagFilter::Linear, SamplerAddressMode::Clamp))
			.SetTexture("hatch_r", _hatchingsPatternR, Sampler(SamplerMinMagFilter::Nearest, SamplerAddressMode::Repeat))
			.SetTexture("hatch_g", _hatchingsPatternG, Sampler(SamplerMinMagFilter::Nearest, SamplerAddressMode::Repeat))
			.SetTexture("hatch_b", _hatchingsPatternB, Sampler(SamplerMinMagFilter::Nearest, SamplerAddressMode::Repeat))
			.SetUniform("hatch_scale", 16.0f * _gc->GetCombinedScaling())
			.Render();
	};
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

		_shadowVertices.AddVertex({p1});
		_shadowVertices.AddVertex({p2});
		_shadowVertices.AddVertex({p3});
		_shadowVertices.AddVertex({p3});
		_shadowVertices.AddVertex({p4});
		_shadowVertices.AddVertex({p1});
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

		_skirtVertices.AddVertex({glm::vec3{p, h + 0.5f}, h});
		_skirtVertices.AddVertex({glm::vec3{p, -2.5f}, h});
	}

	_skirtVertices.AddVertex(_skirtVertices.GetVertices()[0]);
	_skirtVertices.AddVertex(_skirtVertices.GetVertices()[1]);
}


void SmoothTerrainRenderer::UpdateSplatmap()
{
	int width = _smoothGroundMap->GetImage()->GetWidth();
	int height = _smoothGroundMap->GetImage()->GetHeight();

	GLubyte* data = new GLubyte[4 * width * height];
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

	_splatmap->LoadTextureFromData(width, height, data);
	_splatmap->GenerateMipmap();

	delete[] data;
}


void SmoothTerrainRenderer::UpdateChanges(bounds2f bounds)
{
	InitializeSkirt();
	UpdateSplatmap();

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
					_lineVertices.AddVertex({{x0, y0, h00}});
					_lineVertices.AddVertex({{x2, y0, h20}});
				}
				float y2, h02;
				if (y != n)
				{
					y2 = corner.y + size.y * ((y + 2) / k);
					h02 = _smoothGroundMap->GetHeightMap()->GetHeight(x, y + 2);
					_lineVertices.AddVertex({{x0, y0, h00}});
					_lineVertices.AddVertex({{x0, y2, h02}});
				}

				if (x != n && y != n)
				{
					float x1 = corner.x + size.x * ((x + 1) / k);
					float y1 = corner.y + size.y * ((y + 1) / k);
					float h11 = _smoothGroundMap->GetHeightMap()->GetHeight(x + 1, y + 1);
					float h22 = _smoothGroundMap->GetHeightMap()->GetHeight(x + 2, y + 2);

					_lineVertices.AddVertex({{x0, y0, h00}});
					_lineVertices.AddVertex({{x1, y1, h11}});
					_lineVertices.AddVertex({{x2, y0, h20}});
					_lineVertices.AddVertex({{x1, y1, h11}});
					_lineVertices.AddVertex({{x0, y2, h02}});
					_lineVertices.AddVertex({{x1, y1, h11}});
					_lineVertices.AddVertex({{x2, y2, h22}});
					_lineVertices.AddVertex({{x1, y1, h11}});
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


static int _colorScheme = 0;


static glm::vec3 heightcolor(float h)
{
	static std::vector<std::pair<float, glm::vec3>>* _colors = nullptr;
	if (_colors == nullptr)
	{
		_colors = new std::vector<std::pair<float, glm::vec3>>();
		switch (_colorScheme)
		{
			case 1:
				_colors->push_back(std::pair<float, glm::vec3>(-2.5, glm::vec3(164, 146, 124) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(-0.5, glm::vec3(219, 186, 153) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(0.0,  glm::vec3(191, 171, 129) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(6.5,  glm::vec3(114, 150, 65) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(7.0,  glm::vec3(120, 150, 64) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(10,   glm::vec3(135, 149, 60) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(50,   glm::vec3(132, 137, 11) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(150,  glm::vec3(132, 137, 11) / 255.0f));
				break;

			case 2: // Granicus
				_colors->push_back(std::pair<float, glm::vec3>(-2.5, glm::vec3(156, 137, 116) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(0.5, glm::vec3(156, 137, 116) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(1.0,  glm::vec3(128, 137, 74) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(50,  glm::vec3(72, 67, 38) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(150,  glm::vec3(72, 67, 38) / 255.0f));
				break;

			case 3: // Issus
				_colors->push_back(std::pair<float, glm::vec3>(-2.5, glm::vec3(204, 168, 146) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(0.5, glm::vec3(204, 168, 146) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(1.0,  glm::vec3(221, 138, 88) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(50,  glm::vec3(197, 111, 60) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(150,  glm::vec3(197, 111, 60) / 255.0f));
				break;

			case 4: // Hydaspes
				_colors->push_back(std::pair<float, glm::vec3>(-2.5, glm::vec3(138, 153, 105) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(0.5, glm::vec3(144, 149, 110) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(1.0,  glm::vec3(128, 137, 74) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(50,  glm::vec3(72, 67, 38) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(150,  glm::vec3(72, 67, 38) / 255.0f));
				break;

			default: // samurai
				_colors->push_back(std::pair<float, glm::vec3>(-2.5, glm::vec3(164, 146, 124) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(-0.5, glm::vec3(219, 186, 153) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(0.0,  glm::vec3(194, 142, 102) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(6.5,  glm::vec3(199, 172, 148) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(7.0,  glm::vec3(177, 172, 132) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(10,   glm::vec3(125, 171, 142) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(50,   glm::vec3(59,  137,  11) / 255.0f));
				_colors->push_back(std::pair<float, glm::vec3>(150,  glm::vec3(59,  137,  11) / 255.0f));
				break;
		}
	}

	int i = 0;
	while (i + 1 < (int)_colors->size() && h > (*_colors)[i + 1].first)
		++i;

	float h1 = (*_colors)[i].first;
	float h2 = (*_colors)[i + 1].first;
	glm::vec3 c1 = (*_colors)[i].second;
	glm::vec3 c2 = (*_colors)[i + 1].second;

	return glm::mix(c1, c2, (bounds1f(h1, h2).clamp(h) - h1) / (h2 - h1));
}


static glm::vec3 adjust_brightness(glm::vec3 c, float brightness)
{
	if (brightness < 0.5f)
		return c * (1.0f - 0.2f * (0.5f - brightness));
	else if (brightness > 0.83f)
		return glm::mix(c, glm::vec3(1.0f), 0.3f * (brightness - 0.5f));
	else
		return glm::mix(c, glm::vec3(1.0f), 0.2f * (brightness - 0.5f));
}


Texture* SmoothTerrainRenderer::CreateColorMap(GraphicsContext* gc)
{
	static Image* image = nullptr;
	if (image == nullptr)
	{
		static glm::vec3 r[256];
		for (int i = 0; i < 256; ++i)
		{
			r[i].r = (std::rand() & 0x7fff) / (float)0x7fff;
			r[i].g = (std::rand() & 0x7fff) / (float)0x7fff;
			r[i].b = (std::rand() & 0x7fff) / (float)0x7fff;
		}

		image = new Image(64, 256);
		for (int y = 0; y < 256; ++y)
			for (int x = 0; x < 64; ++x)
			{
				float brightness = x / 63.0f;
				float h = -2.5f + 0.5f * y;
				glm::vec3 c = heightcolor(h);
				c = adjust_brightness(c, brightness);
				if (h > 0)
					c = glm::mix(c, r[y], 0.015f);

				image->SetPixel(x, 255 - y, glm::vec4(c, 1));
			}
	}

	Texture* result = new Texture(gc);
	result->LoadTextureFromImage(*image);

	return result;
}
