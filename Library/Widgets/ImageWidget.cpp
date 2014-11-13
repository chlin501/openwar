// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ImageWidget.h"



ImageWidget::ImageWidget() :
	_alpha(1)
{
}


std::shared_ptr<TextureImage> ImageWidget::GetTextureImage() const
{
	return _textureImage;
}


void ImageWidget::SetTextureImage(std::shared_ptr<TextureImage> textureImage)
{
	_textureImage = textureImage;
}


BorderBounds ImageWidget::GetBounds() const
{
	return _bounds;
}


void ImageWidget::SetBounds(const BorderBounds& value)
{
	_bounds = value;
}


glm::vec4 ImageWidget::GetColorize() const
{
	return _colorize;
}


void ImageWidget::SetColorize(const glm::vec4& value)
{
	_colorize = value;
}


float ImageWidget::GetAlpha() const
{
	return _alpha;
}


void ImageWidget::SetAlpha(float value)
{
	_alpha = value;
}


void ImageWidget::AppendVertices(WidgetView* widgetView, std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
	if (_textureImage == nullptr || _bounds.outer.empty())
		return;

	BorderBounds texture = _textureImage->GetCoords();

	bool min_x = _bounds.outer.min.x < _bounds.inner.min.x;
	bool max_x = _bounds.inner.max.x < _bounds.outer.max.x;
	bool min_y = _bounds.outer.min.y < _bounds.inner.min.y;
	bool max_y = _bounds.inner.max.y < _bounds.outer.max.y;

	if (min_x && min_y)
		AppendRectangle(vertices,
			bounds2f(_bounds.outer.min.x, _bounds.outer.min.y, _bounds.inner.min.x, _bounds.inner.min.y),
			bounds2f(texture.outer.min.x, texture.outer.max.y, texture.inner.min.x, texture.inner.max.y));

	if (min_x)
		AppendRectangle(vertices,
			bounds2f(_bounds.outer.min.x, _bounds.inner.min.y, _bounds.inner.min.x, _bounds.inner.max.y),
			bounds2f(texture.outer.min.x, texture.inner.max.y, texture.inner.min.x, texture.inner.min.y));

	if (min_x && max_y)
		AppendRectangle(vertices,
			bounds2f(_bounds.outer.min.x, _bounds.inner.max.y, _bounds.inner.min.x, _bounds.outer.max.y),
			bounds2f(texture.outer.min.x, texture.inner.min.y, texture.inner.min.x, texture.outer.min.y));

	if (min_y)
		AppendRectangle(vertices,
			bounds2f(_bounds.inner.min.x, _bounds.outer.min.y, _bounds.inner.max.x, _bounds.inner.min.y),
			bounds2f(texture.inner.min.x, texture.outer.max.y, texture.inner.max.x, texture.inner.max.y));

	AppendRectangle(vertices,
		bounds2f(_bounds.inner.min.x, _bounds.inner.min.y, _bounds.inner.max.x, _bounds.inner.max.y),
		bounds2f(texture.inner.min.x, texture.inner.max.y, texture.inner.max.x, texture.inner.min.y));

	if (max_y)
		AppendRectangle(vertices,
			bounds2f(_bounds.inner.min.x, _bounds.inner.max.y, _bounds.inner.max.x, _bounds.outer.max.y),
			bounds2f(texture.inner.min.x, texture.inner.min.y, texture.inner.max.x, texture.outer.min.y));

	if (max_x && min_y)
		AppendRectangle(vertices,
			bounds2f(_bounds.inner.max.x, _bounds.outer.min.y, _bounds.outer.max.x, _bounds.inner.min.y),
			bounds2f(texture.inner.max.x, texture.outer.max.y, texture.outer.max.x, texture.inner.max.y));

	if (max_x)
		AppendRectangle(vertices,
			bounds2f(_bounds.inner.max.x, _bounds.inner.min.y, _bounds.outer.max.x, _bounds.inner.max.y),
			bounds2f(texture.inner.max.x, texture.inner.max.y, texture.outer.max.x, texture.inner.min.y));

	if (max_x && max_y)
		AppendRectangle(vertices,
			bounds2f(_bounds.inner.max.x, _bounds.inner.max.y, _bounds.outer.max.x, _bounds.outer.max.y),
			bounds2f(texture.inner.max.x, texture.inner.min.y, texture.outer.max.x, texture.outer.min.y));
}


void ImageWidget::AppendRectangle(std::vector<Vertex_2f_2f_4f_1f>& vertices, bounds2f xy, bounds2f uv)
{
	vertices.push_back(Vertex_2f_2f_4f_1f(glm::vec2(xy.min.x, xy.min.y), glm::vec2(uv.min.x, uv.min.y), _colorize, _alpha));
	vertices.push_back(Vertex_2f_2f_4f_1f(glm::vec2(xy.min.x, xy.max.y), glm::vec2(uv.min.x, uv.max.y), _colorize, _alpha));
	vertices.push_back(Vertex_2f_2f_4f_1f(glm::vec2(xy.max.x, xy.max.y), glm::vec2(uv.max.x, uv.max.y), _colorize, _alpha));
	vertices.push_back(Vertex_2f_2f_4f_1f(glm::vec2(xy.max.x, xy.max.y), glm::vec2(uv.max.x, uv.max.y), _colorize, _alpha));
	vertices.push_back(Vertex_2f_2f_4f_1f(glm::vec2(xy.max.x, xy.min.y), glm::vec2(uv.max.x, uv.min.y), _colorize, _alpha));
	vertices.push_back(Vertex_2f_2f_4f_1f(glm::vec2(xy.min.x, xy.min.y), glm::vec2(uv.min.x, uv.min.y), _colorize, _alpha));
}
