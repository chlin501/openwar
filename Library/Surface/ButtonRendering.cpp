// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonRendering.h"
#include "ButtonGrid.h"
#include "Shapes/VertexShape.h"
#include "GraphicsContext.h"
#include "TextureResource.h"
#include <glm/gtc/matrix_transform.hpp>
#include "WidgetShape.h"


ButtonRendering::ButtonRendering(GraphicsContext* gc) :
_gc(gc)
{
	_textureButtonBackground = new TextureAtlas(gc);
	_textureButtonBackground->LoadAtlasFromResource(resource("Textures/ButtonNormal.png"));

	_textureButtonHighlight = new TextureAtlas(gc);
	_textureButtonHighlight->LoadAtlasFromResource(resource("Textures/ButtonHighlight.png"));

	_textureButtonSelected = new TextureAtlas(gc);
	_textureButtonSelected->LoadAtlasFromResource(resource("Textures/ButtonSelected.png"));

	_textureButtonIcons = new TextureAtlas(gc);
	_textureButtonIcons->LoadAtlasFromResource(resource("Textures/ButtonIcons.png"));

	_textureEditorTools = new TextureAtlas(gc);
	_textureEditorTools->LoadAtlasFromResource(resource("Textures/EditorTools.png"));

	_string_font = new TextureFont(gc->GetWidgetTextureAtlas(), FontDescriptor(true, 18));
	_string_shape = new WidgetShape(gc->GetWidgetTextureAtlas());

	buttonIconPlay = new TextureImageX(_textureButtonIcons, glm::vec2(25, 32), bounds2f(0, 0, 25, 32) / glm::vec2(128, 32));
	buttonIconPause = new TextureImageX(_textureButtonIcons, glm::vec2(25, 32), bounds2f(25, 0, 50, 32) / glm::vec2(128, 32));

	buttonEditorToolHand = new TextureImageX(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.00, 0.0, 0.25, 0.5));
	buttonEditorToolPaint = new TextureImageX(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.25, 0.0, 0.50, 0.5));
	buttonEditorToolErase = new TextureImageX(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.50, 0.0, 0.75, 0.5));
	buttonEditorToolSmear = new TextureImageX(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.75, 0.0, 1.00, 0.5));
	buttonEditorToolHills = new TextureImageX(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.00, 0.5, 0.25, 1.0));
	buttonEditorToolTrees = new TextureImageX(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.25, 0.5, 0.50, 1.0));
	buttonEditorToolWater = new TextureImageX(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.50, 0.5, 0.75, 1.0));
	buttonEditorToolFords = new TextureImageX(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.75, 0.5, 1.00, 1.0));
}


void ButtonRendering::RenderTexturePatch(const glm::mat4& transform, TextureAtlas* textureAtlas, bounds2f outer_xy, float radius)
{
	PatchGlyph patchGlyph;

	patchGlyph.outer_xy = outer_xy;
	patchGlyph.inner_xy = outer_xy.grow(-radius);
	patchGlyph.outer_uv = bounds2f(0, 0, 1, 1);
	patchGlyph.inner_uv = bounds2f(0.5f, 0.5f, 0.5f, 0.5f);

	WidgetShape widgetShape(textureAtlas);
	widgetShape.AddGlyph(&patchGlyph);

	RenderCall<WidgetShader> renderCall(_gc);

	renderCall.SetVertices(widgetShape.GetVertices());
	renderCall.SetTexture("texture", textureAtlas);
	renderCall.SetUniform("transform", transform);
	renderCall.SetUniform("color", glm::vec4(1, 1, 1, 1));
	renderCall.Render();
}


void ButtonRendering::RenderButtonIcon(const glm::mat4& transform, glm::vec2 position, TextureImageX* buttonIcon, bool disabled)
{
	if (buttonIcon != nullptr)
	{
		bounds2f bounds = buttonIcon->GetOuterBounds();
		bounds += position - bounds.center();

		RenderTextureImage(transform, buttonIcon->_textureAtlas, bounds, buttonIcon->GetOuterUV(), disabled ? 0.5f : 1.0f);
	}
}


void ButtonRendering::RenderTextureImage(const glm::mat4& transform, TextureAtlas* textureAtlas, bounds2f bounds_xy, bounds2f bounds_uv, float alpha)
{
	PatchGlyph patchGlyph;

	patchGlyph.outer_xy = bounds_xy;
	patchGlyph.inner_xy = bounds_xy;
	patchGlyph.outer_uv = bounds_uv;
	patchGlyph.inner_uv = bounds_uv;
	patchGlyph._alpha = alpha;

	WidgetShape widgetShape(textureAtlas);
	widgetShape.AddGlyph(&patchGlyph);

	RenderCall<WidgetShader> renderCall(_gc);

	renderCall.SetVertices(widgetShape.GetVertices());
	renderCall.SetTexture("texture", textureAtlas);
	renderCall.SetUniform("transform", transform);
	renderCall.SetUniform("color", glm::vec4(1, 1, 1, 1));
	renderCall.Render();
}


void ButtonRendering::RenderStringGlyph(const glm::mat4& transform, glm::vec2 position, const char* text)
{
	StringGlyph stringGlyph(text, glm::mat4x4());
	_string_shape->AddGlyph(&stringGlyph);

	glm::vec2 p = position - 0.5f * _string_shape->MeasureGlyph(&stringGlyph) - glm::vec2(0, 1);

	RenderCall<WidgetShader> renderCall(_gc);

	renderCall.SetVertices(_string_shape->GetVertices());
	renderCall.SetTexture("texture", _gc->GetWidgetTextureAtlas());
	renderCall.SetUniform("transform", glm::translate(transform, glm::vec3(p, 0)));
	renderCall.SetUniform("color", glm::vec4(1, 1, 1, 1));
	renderCall.Render();
}
