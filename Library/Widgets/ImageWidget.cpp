// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ImageWidget.h"
#include "Surface/Touch.h"



ImageWidget::ImageWidget(WidgetOwner* widgetOwner) : Widget(widgetOwner),
	_alpha(1)
{
}


bounds2f ImageWidget::GetBounds() const
{
	return _bounds;
}


void ImageWidget::SetBounds(const bounds2f& value)
{
	_bounds = value;
}


std::shared_ptr<TextureImage> ImageWidget::GetTextureImage() const
{
	return _textureImage;
}


void ImageWidget::SetTextureImage(std::shared_ptr<TextureImage> textureImage)
{
	_textureImage = textureImage;
}


BorderInset ImageWidget::GetInset() const
{
	return _inset;
}


void ImageWidget::SetInset(BorderInset value)
{
	_inset = value;
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


void ImageWidget::OnTouchEnter(Touch* touch)
{
}


void ImageWidget::OnTouchBegin(Touch* touch)
{
}


void ImageWidget::RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
	if (_textureImage == nullptr)
		return;

	bounds2f outer = _bounds;
	if (outer.empty())
		return;

	bounds2f inner(outer.min + _inset.min, outer.max - _inset.max);
	inner.min.x = std::min(inner.min.x, outer.max.x);
	inner.max.x = std::max(inner.max.x, outer.min.x);
	inner.min.y = std::min(inner.min.y, outer.max.y);
	inner.max.y = std::max(inner.max.y, outer.min.y);
	if (inner.min.x > inner.max.x)
		inner.min.x = inner.max.x = (inner.min.x + inner.max.x) / 2;
	if (inner.min.y > inner.max.y)
		inner.min.y = inner.max.y = (inner.min.y + inner.max.y) / 2;

	BorderBounds texture = _textureImage->GetCoords();

	bool min_x = outer.min.x < inner.min.x;
	bool max_x = inner.max.x < outer.max.x;
	bool min_y = outer.min.y < inner.min.y;
	bool max_y = inner.max.y < outer.max.y;

	if (min_x && min_y)
		AppendRectangle(vertices,
			bounds2f(outer.min.x, outer.min.y, inner.min.x, inner.min.y),
			bounds2f(texture.outer.min.x, texture.outer.max.y, texture.inner.min.x, texture.inner.max.y));

	if (min_x)
		AppendRectangle(vertices,
			bounds2f(outer.min.x, inner.min.y, inner.min.x, inner.max.y),
			bounds2f(texture.outer.min.x, texture.inner.max.y, texture.inner.min.x, texture.inner.min.y));

	if (min_x && max_y)
		AppendRectangle(vertices,
			bounds2f(outer.min.x, inner.max.y, inner.min.x, outer.max.y),
			bounds2f(texture.outer.min.x, texture.inner.min.y, texture.inner.min.x, texture.outer.min.y));

	if (min_y)
		AppendRectangle(vertices,
			bounds2f(inner.min.x, outer.min.y, inner.max.x, inner.min.y),
			bounds2f(texture.inner.min.x, texture.outer.max.y, texture.inner.max.x, texture.inner.max.y));

	if (!inner.empty())
		AppendRectangle(vertices,
			bounds2f(inner.min.x, inner.min.y, inner.max.x, inner.max.y),
			bounds2f(texture.inner.min.x, texture.inner.max.y, texture.inner.max.x, texture.inner.min.y));

	if (max_y)
		AppendRectangle(vertices,
			bounds2f(inner.min.x, inner.max.y, inner.max.x, outer.max.y),
			bounds2f(texture.inner.min.x, texture.inner.min.y, texture.inner.max.x, texture.outer.min.y));

	if (max_x && min_y)
		AppendRectangle(vertices,
			bounds2f(inner.max.x, outer.min.y, outer.max.x, inner.min.y),
			bounds2f(texture.inner.max.x, texture.outer.max.y, texture.outer.max.x, texture.inner.max.y));

	if (max_x)
		AppendRectangle(vertices,
			bounds2f(inner.max.x, inner.min.y, outer.max.x, inner.max.y),
			bounds2f(texture.inner.max.x, texture.inner.max.y, texture.outer.max.x, texture.inner.min.y));

	if (max_x && max_y)
		AppendRectangle(vertices,
			bounds2f(inner.max.x, inner.max.y, outer.max.x, outer.max.y),
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
