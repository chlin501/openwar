// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ImageWidget_H
#define ImageWidget_H

#include "Widget.h"
#include "TextureAtlas.h"


class ImageWidget : public Widget
{
	bounds2f _bounds;
	std::shared_ptr<TextureImage> _textureImage;
	BorderInset _inset;
	glm::vec4 _colorize;
	float _alpha;

public:
	ImageWidget(WidgetOwner* widgetOwner);

	bounds2f GetBounds() const;
	void SetBounds(const bounds2f& value);

	std::shared_ptr<TextureImage> GetTextureImage() const;
	void SetTextureImage(std::shared_ptr<TextureImage> textureImage);

	void SetInset(BorderInset value);

	glm::vec4 GetColorize() const;
	void SetColorize(const glm::vec4& value);

	float GetAlpha() const;
	void SetAlpha(float value);

	// Widget

	virtual void OnTouchEnter(Touch* touch);
	virtual void OnTouchBegin(Touch* touch);

	virtual void AppendVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

private:
	void AppendRectangle(std::vector<Vertex_2f_2f_4f_1f>& vertices, bounds2f xy, bounds2f uv);

private:
	ImageWidget(const ImageWidget&) : Widget(nullptr) { }
	ImageWidget& operator=(const ImageWidget&) { return *this; }
};


#endif
