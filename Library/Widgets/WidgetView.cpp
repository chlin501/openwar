// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "WidgetView.h"
#include "GraphicsContext.h"
#include "WidgetShape.h"
#include "ScrollerViewport.h"
#include "RenderCall.h"
#include "WidgetShader.h"
#include "TextureAtlas.h"


WidgetView::WidgetView(GraphicsContext* gc) :
	_gc(gc),
	_viewport(nullptr),
	_widgetShape(nullptr)
{
	_viewport = new ScrollerViewport(_gc);
	_widgetShape = new WidgetShape(_gc->GetWidgetTextureAtlas());
}


WidgetView::~WidgetView()
{
}


ScrollerViewport* WidgetView::GetViewport() const
{
	return _viewport;
}


WidgetShape* WidgetView::GetWidgetShape() const
{
	return _widgetShape;
}


void WidgetView::Render()
{
	_viewport->UseViewport();

	RenderCall<WidgetShader>(_gc)
		.SetVertices(_widgetShape->GetVertices(), "position", "texcoord", "colorize", "alpha")
		.SetUniform("transform", _viewport->GetTransform())
		.SetTexture("texture", _widgetShape->GetTextureAtlas())
		.Render();
}
