// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ImageWidget_H
#define ImageWidget_H

#include "WidgetShape.h"


class ImageWidget : public Widget
{
public:
	bounds2f outer_xy;
	bounds2f inner_xy;
	bounds2f outer_uv;
	bounds2f inner_uv;
	glm::vec4 _colorize;
	float _alpha;

	ImageWidget();
	ImageWidget(std::shared_ptr<TextureImage> tile, bounds2f bounds, glm::vec2 inset);

	void Reset();
	void Reset(std::shared_ptr<TextureImage> tile, bounds2f bounds, glm::vec2 inset);

private:
	virtual void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

	void AppendRectangle(std::vector<Vertex_2f_2f_4f_1f>& vertices, bounds2f xy, bounds2f uv);

private:
	ImageWidget(const ImageWidget&) { }
	ImageWidget& operator=(const ImageWidget&) { return *this; }
};


#endif
