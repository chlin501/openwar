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

	virtual bounds2f GetBounds() const;
	virtual void SetBounds(const bounds2f& value);

	virtual std::shared_ptr<TextureImage> GetTextureImage() const;
	virtual void SetTextureImage(std::shared_ptr<TextureImage> textureImage);

	virtual void SetInset(BorderInset value);

	virtual glm::vec4 GetColorize() const;
	virtual void SetColorize(const glm::vec4& value);

	virtual float GetAlpha() const;
	virtual void SetAlpha(float value);

	// Widget

	virtual void OnTouchEnter(Touch* touch);
	virtual void OnTouchBegin(Touch* touch);

	virtual void RenderVertices(std::vector<Vertex_2f_2f_4f_1f>& vertices);

private:
	void AppendRectangle(std::vector<Vertex_2f_2f_4f_1f>& vertices, bounds2f xy, bounds2f uv);

private:
	ImageWidget(const ImageWidget&) : Widget(nullptr) { }
	ImageWidget& operator=(const ImageWidget&) { return *this; }
};


#endif
