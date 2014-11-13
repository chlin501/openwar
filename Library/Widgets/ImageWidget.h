// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ImageWidget_H
#define ImageWidget_H

#include "WidgetShape.h"


class ImageWidget : public Widget
{
	std::shared_ptr<TextureImage> _textureImage;
	BorderBounds _bounds;
	glm::vec4 _colorize;
	float _alpha;

public:
	ImageWidget();

	std::shared_ptr<TextureImage> GetTextureImage() const;
	void SetTextureImage(std::shared_ptr<TextureImage> textureImage);

	BorderBounds GetBounds() const;
	void SetBounds(const BorderBounds& value);

	glm::vec4 GetColorize() const;
	void SetColorize(const glm::vec4& value);

	float GetAlpha() const;
	void SetAlpha(float value);

protected:
	virtual void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

private:
	void AppendRectangle(std::vector<Vertex_2f_2f_4f_1f>& vertices, bounds2f xy, bounds2f uv);

private:
	ImageWidget(const ImageWidget&) { }
	ImageWidget& operator=(const ImageWidget&) { return *this; }
};


#endif
