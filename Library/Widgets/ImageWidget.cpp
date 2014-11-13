#include "ImageWidget.h"



ImageWidget::ImageWidget() :
	_alpha(1)
{

}


ImageWidget::ImageWidget(std::shared_ptr<TextureImage> tile, bounds2f bounds, glm::vec2 inset) :
	_alpha(1)
{
	outer_xy = bounds;
	inner_xy = bounds.grow(-inset.x, -inset.y);
	outer_uv = tile->GetOuterUV();
	inner_uv = tile->GetInnerUV();
}


void ImageWidget::Reset()
{
	outer_xy = bounds2f();
	inner_xy = bounds2f();
	outer_uv = bounds2f();
	inner_uv = bounds2f();
}


void ImageWidget::Reset(std::shared_ptr<TextureImage> tile, bounds2f bounds, glm::vec2 inset)
{
	outer_xy = bounds;
	inner_xy = bounds.grow(-inset.x, -inset.y);
	outer_uv = tile->GetOuterUV();
	inner_uv = tile->GetInnerUV();
}


void ImageWidget::AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices)
{
	bounds2f ixy = inner_xy;
	bounds2f oxy = outer_xy;
	bounds2f iuv = inner_uv;
	bounds2f ouv = outer_uv;

	bool min_x = oxy.min.x < ixy.min.x;
	bool max_x = ixy.max.x < oxy.max.x;
	bool min_y = oxy.min.y < ixy.min.y;
	bool max_y = ixy.max.y < oxy.max.y;

	if (min_x && min_y)
		AppendRectangle(vertices,
			bounds2f(oxy.min.x, oxy.min.y, ixy.min.x, ixy.min.y),
			bounds2f(ouv.min.x, ouv.max.y, iuv.min.x, iuv.max.y));

	if (min_x)
		AppendRectangle(vertices,
			bounds2f(oxy.min.x, ixy.min.y, ixy.min.x, ixy.max.y),
			bounds2f(ouv.min.x, iuv.max.y, iuv.min.x, iuv.min.y));

	if (min_x && max_y)
		AppendRectangle(vertices,
			bounds2f(oxy.min.x, ixy.max.y, ixy.min.x, oxy.max.y),
			bounds2f(ouv.min.x, iuv.min.y, iuv.min.x, ouv.min.y));

	if (min_y)
		AppendRectangle(vertices,
			bounds2f(ixy.min.x, oxy.min.y, ixy.max.x, ixy.min.y),
			bounds2f(iuv.min.x, ouv.max.y, iuv.max.x, iuv.max.y));

	AppendRectangle(vertices,
		bounds2f(ixy.min.x, ixy.min.y, ixy.max.x, ixy.max.y),
		bounds2f(iuv.min.x, iuv.max.y, iuv.max.x, iuv.min.y));

	if (max_y)
		AppendRectangle(vertices,
			bounds2f(ixy.min.x, ixy.max.y, ixy.max.x, oxy.max.y),
			bounds2f(iuv.min.x, iuv.min.y, iuv.max.x, ouv.min.y));

	if (max_x && min_y)
		AppendRectangle(vertices,
			bounds2f(ixy.max.x, oxy.min.y, oxy.max.x, ixy.min.y),
			bounds2f(iuv.max.x, ouv.max.y, ouv.max.x, iuv.max.y));

	if (max_x)
		AppendRectangle(vertices,
			bounds2f(ixy.max.x, ixy.min.y, oxy.max.x, ixy.max.y),
			bounds2f(iuv.max.x, iuv.max.y, ouv.max.x, iuv.min.y));

	if (max_x && max_y)
		AppendRectangle(vertices,
			bounds2f(ixy.max.x, ixy.max.y, oxy.max.x, oxy.max.y),
			bounds2f(iuv.max.x, iuv.min.y, ouv.max.x, ouv.min.y));
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
